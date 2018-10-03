#include <algorithm>

#include "RenderContext.h"

namespace {

// component-wise multiplication (borrowed from GLSL)
template <typename VALUE>
Vec4T<VALUE> operator*(const Vec4T<VALUE> &vec1, const Vec4T<VALUE> &vec2)
{
  return Vec4T<VALUE>(
    vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z, vec1.w * vec2.w);
}

// special operator to multiply RGBA values
template <typename VALUE>
uint32 operator*(const Vec4T<VALUE> &color1, uint32 color2)
{
  return ((uint32)((color2 & 0xff000000) * color1.w) & (uint32)0xff000000)
    | ((uint32)((color2 & 0x00ff0000) * color1.z) & (uint32)0x00ff0000)
    | ((uint32)((color2 & 0x0000ff00) * color1.y) & (uint32)0x0000ff00)
    | ((uint32)((color2 & 0x000000ff) * color1.x) & (uint32)0x000000ff);
}

template <typename VALUE>
Vec4T<VALUE> clamp(const Vec4T<VALUE> &value, VALUE min, VALUE max)
{
  return Vec4T<VALUE>(
    ::clamp(value.x, min, max), ::clamp(value.y, min, max),
    ::clamp(value.z, min, max), ::clamp(value.w, min, max));
}

static uint32 black = 0x00000000;

} // namespace

RenderContext::RenderContext(uint width, uint height):
  _width(width), _height(height),
  _rgbaClear(0x00000000), _depthClear(1.0f),
  _fb(_width * _height, _rgbaClear, _depthClear),
  _matScreen(
    Mat4x4f(InitScale, 0.5f * _width, -0.5f * _height, 1.0f)
    * Mat4x4f(InitTrans, Vec3f(1.0f, -1.0f, 0.0f))),
  _matProj(Mat4x4f(InitIdent)),
  _matView(Mat4x4f(InitIdent)),
  _matCam(Mat4x4f(InitIdent)),
  _matModel(Mat4x4f(InitIdent)),
  _normal(0.0f, 0.0f, 1.0f),
  _color(1.0f, 1.0f, 1.0f, 1.0f),
  _texCoord(0.0f, 0.0f),
  _light(0.0f, 0.0f, 1.0f),
  _ambient(0.2f),
  _mode(1 << FrontSide),
  _iTex(0),
  _vtxColor(false), _vtxNormal(false), _iVtx(0)
{
  _tex.emplace_back(1, 1, &black); // make _iTex[0] valid always
}

void RenderContext::setCamMat(const Mat4x4f &mat)
{
  _matCam = mat; _matView = invert(_matCam);
}

void RenderContext::setViewMat(const Mat4x4f &mat)
{
  _matView = mat; _matCam = invert(_matView);
}

void RenderContext::enable(Mode mode, bool enable)
{
  (_mode &= ~(1 << mode)) |= ((uint)enable << mode);
}

namespace {

Vec4f lighting(
  const Vec4f &color, const Vec3f &normal,
  const Vec3f &light, const float ambient)
{
  float f = dot(light, normal);
  if (f < 0.0f) f = 0.0f;
  f = ambient + (1.0f - ambient) * f;
  return Vec4f(color.x * f, color.y * f, color.z * f, color.w);
  // alpha is uneffected by lighting
}

} // namespace

void RenderContext::drawVertex(const Vec3f &coord)
{
  assert(_iVtx < 3);
  { Vertex &vtx = _vtcs[_iVtx];
    const Mat4x4f matMVPS
      = _matScreen * _matProj * _matView * _matModel;
    vtx.coord = transformPoint(matMVPS, coord);
    vtx.normal = transformVec(_matModel, _normal);
    vtx.color = _color; vtx.texCoord = _texCoord;
  }
  if (++_iVtx == 3) {
    // reset per vertex states
    _iVtx = 0; _vtxNormal = _vtxColor = false;
    // face-culling / light correction
    Vec3f light = _light;
    // determine face normal
    const Vec3f normal
      = cross(
        _vtcs[1].coord - _vtcs[0].coord,
        _vtcs[1].coord - _vtcs[2].coord);
    if (normal.z < 0) { // view at back of face 
      if (!isEnabled(BackSide)) return;
      light = -_light;
    } else { // view at front of face
      if (!isEnabled(FrontSide)) return;
    }
    /// @todo frustum culling
    // lighting
    if (isEnabled(Lighting)) {
      _vtcs[0].color
        = lighting(_vtcs[0].color, _vtcs[0].normal, light, _ambient);
      _vtcs[1].color
        = lighting(_vtcs[1].color, _vtcs[1].normal, light, _ambient);
      _vtcs[2].color
        = lighting(_vtcs[2].color, _vtcs[2].normal, light, _ambient);
    }
    // call rasterize
    typedef
      void(RenderContext::*Rasterize)(const Vertex(&)[3]);
    static const Rasterize rasterizes[] = {
      &RenderContext::rasterize<
        NoDepth, false, false, false>,
      &RenderContext::rasterize<
        DepthWrite, false, false, false>,
      &RenderContext::rasterize<
        DepthCheckAndWrite, false, false, false>,
      &RenderContext::rasterize<
        NoDepth, true, false, false>,
      &RenderContext::rasterize<
        DepthWrite, true, false, false>,
      &RenderContext::rasterize<
        DepthCheckAndWrite, true, false, false>,
      &RenderContext::rasterize<
        NoDepth, false, true, false>,
      &RenderContext::rasterize<
        DepthWrite, false, true, false>,
      &RenderContext::rasterize<
        DepthCheckAndWrite, false, true, false>,
      &RenderContext::rasterize<
        NoDepth, true, true, false>,
      &RenderContext::rasterize<
        DepthWrite, true, true, false>,
      &RenderContext::rasterize<
        DepthCheckAndWrite, true, true, false>,
      &RenderContext::rasterize<
        NoDepth, false, false, true>,
      &RenderContext::rasterize<
        DepthWrite, false, false, true>,
      &RenderContext::rasterize<
        DepthCheckAndWrite, false, false, true>,
      &RenderContext::rasterize<
        NoDepth, true, false, true>,
      &RenderContext::rasterize<
        DepthWrite, true, false, true>,
      &RenderContext::rasterize<
        DepthCheckAndWrite, true, false, true>,
      &RenderContext::rasterize<
        NoDepth, false, true, true>,
      &RenderContext::rasterize<
        DepthWrite, false, true, true>,
      &RenderContext::rasterize<
        DepthCheckAndWrite, false, true, true>,
      &RenderContext::rasterize<
        NoDepth, true, true, true>,
      &RenderContext::rasterize<
        DepthWrite, true, true, true>,
      &RenderContext::rasterize<
        DepthCheckAndWrite, true, true, true>
    };
    enum { N = sizeof rasterizes / sizeof *rasterizes };
    const uint tex = (_mode & 1 << Texturing) != 0;
    const uint blend = (_mode & 1 << Blending) != 0;
    const uint smooth = (_mode & 1 << Smooth) != 0;
    const uint depthMode
      = (_mode & 1 << DepthBuffer) != 0
      ? (_mode & 1 << DepthTest) != 0
      ? DepthCheckAndWrite : DepthWrite : NoDepth;
    const uint i = (((tex * 2) + blend) * 2 + smooth) * 3 + depthMode;
    assert(i < N);
    (this->*rasterizes[i])(_vtcs);
  }
}

uint RenderContext::loadTex(uint width, uint height, const uint32 img[])
{
  // find free texture slot
  uint iTex = 0; const uint nTex = (uint)_tex.size();
  for (; iTex < nTex && !_tex[iTex].empty(); ++iTex);
  if (iTex == nTex) _tex.emplace_back();
  return _tex[iTex].load(width, height, img) ? iTex : 0;
}

uint RenderContext::setTex(uint i)
{
  return _iTex = i < _tex.size() && !_tex[i].empty() ? i : 0;
}

void RenderContext::setClearColor(const Vec4f &color)
{
  _rgbaClear = clamp(color, 0.0f, 1.0f) * 0xffffffff;
}

void RenderContext::clear(bool rgba, bool depth)
{
  if (rgba) std::fill(_fb.rgba.begin(), _fb.rgba.end(), _rgbaClear);
  if (depth) std::fill(_fb.depth.begin(), _fb.depth.end(), _depthClear);
}

template <
  RenderContext::DepthMode DEPTH_MODE,
  bool SMOOTH,
  bool BLEND,
  bool TEX>
void RenderContext::rasterize(const Vertex (&vtcs)[3])
{
  Vec4f color = vtcs[0].color;
  uint32 rgba = color * (uint32)0xffffffff;
  // sort vertices by y coordinates
  uint iVtcs[3] = { 0, 1, 2 };
  if (vtcs[iVtcs[0]].coord.y > vtcs[iVtcs[1]].coord.y) {
    std::swap(iVtcs[0], iVtcs[1]);
  }
  if (vtcs[iVtcs[1]].coord.y > vtcs[iVtcs[2]].coord.y) {
    std::swap(iVtcs[1], iVtcs[2]);
  }
  if (vtcs[iVtcs[0]].coord.y > vtcs[iVtcs[1]].coord.y) {
    std::swap(iVtcs[0], iVtcs[1]);
  }
  // cut triangle in upper and lower part
  const Vertex &vtxT = vtcs[iVtcs[0]];
  const Vertex &vtxM = vtcs[iVtcs[1]];
  const Vertex &vtxB = vtcs[iVtcs[2]];
  if (std::abs(vtxB.coord.y - vtxT.coord.y) < 1E-10) return;
  const float f1
    = (vtxM.coord.y - vtxT.coord.y) / (vtxB.coord.y - vtxT.coord.y);
  const float f0 = 1.0f - f1;
  float xLM = vtxM.coord.x, xRM = lerp(vtxT.coord.x, vtxB.coord.x, f0, f1);
  float zLM, zRM;
  if (DEPTH_MODE > NoDepth) {
    zLM = vtxM.coord.z, zRM = lerp(vtxT.coord.z, vtxB.coord.z, f0, f1);
  }
  Vec4f colorT, colorB, colorLM, colorRM;
  if (SMOOTH) {
    colorT = vtxT.color; colorB = vtxB.color;
    colorLM = vtxM.color; colorRM = lerp(vtxT.color, vtxB.color, f0, f1);
  }
  Vec2f texCoordLM, texCoordRM;
  if (TEX) {
    texCoordLM = vtxM.texCoord;
    texCoordRM = lerp(vtxT.texCoord, vtxB.texCoord, f0, f1);
  }
  if (xLM > xRM) {
    std::swap(xLM, xRM); std::swap(zLM, zRM);
    if (SMOOTH) std::swap(colorLM, colorRM);
    if (TEX) std::swap(texCoordLM, texCoordRM);
  }
  const Texture &tex = _tex[_iTex];
  const int yT = rnd(vtxT.coord.y);
  const int yM = rnd(vtxM.coord.y);
  const int yB = rnd(vtxB.coord.y);
  // draw upper part of triangle
  if (yT < yM) {
    const float dY = vtxM.coord.y - vtxT.coord.y;
    for (int y = std::max(yT, 0), yE = std::min(yM, (int)_height);
      y < yE; ++y) {
      const float f1 = (y - yT) / dY, f0 = 1.0f - f1;
      const float xLY = lerp(vtxT.coord.x, xLM, f0, f1);
      const float xRY = lerp(vtxT.coord.x, xRM, f0, f1);
      const float dX = xRY - xLY;
      const int xL = rnd(xLY), xR = rnd(xRY);
      float zL, zR;
      if (DEPTH_MODE > NoDepth) {
        zL = lerp(vtxT.coord.z, zLM, f0, f1);
        zR = lerp(vtxT.coord.z, zRM, f0, f1);
      }
      Vec4f colorL, colorR;
      if (SMOOTH) {
        colorL = lerp(colorT, colorLM, f0, f1);
        colorR = lerp(colorT, colorRM, f0, f1);
      }
      Vec2f texCoordL, texCoordR;
      if (TEX) {
        texCoordL = lerp(vtxT.texCoord, texCoordLM, f0, f1);
        texCoordR = lerp(vtxT.texCoord, texCoordRM, f0, f1);
      }
      const size_t i = y * _width;
      for (int x = std::max(xL, 0), xE = std::min(xR, (int)_width);
        x < xE; ++x) {
        const size_t iX = i + x;
        float f1, f0;
        if (DEPTH_MODE > NoDepth || SMOOTH || TEX) {
          f1 = (x - xL) / dX; f0 = 1.0f - f1;
        }
        if (DEPTH_MODE > NoDepth) {
          const float z = lerp(zL, zR, f0, f1);
          if (DEPTH_MODE == DepthCheckAndWrite
            && z >= _fb.depth[iX]) continue;
          _fb.depth[iX] = z;
        }
        if (SMOOTH) {
          color = f0 * colorL + f1 * colorR;
        }
        if (TEX) {
          const Vec2f texCoord = f0 * texCoordL + f1 * texCoordR;
          rgba = color * tex[texCoord];
        } else rgba = color * (uint32)0xffffffff;
        if (BLEND) {
          const float f1 = ((rgba >> 24) & 0xff) * 1.0f / 255;
          const float f0 = 1.0f - f1;
          const Vec4f blendFg(f1, f1, f1, f1), blendBg(f0, f0, f0, f0);
          rgba = blendFg * rgba + blendBg * _fb.rgba[iX];
        } else rgba |= 0xff000000;
        _fb.rgba[iX] = rgba;
      }
    }
  }
  // draw lower part of triangle
  if (yM < yB) {
    const float dY = vtxB.coord.y - vtxM.coord.y;
    for (int y = std::max(yM, 0), yE = std::min(yB, (int)_height);
      y < yE; ++y) {
      const float f1 = (y - yM) / dY, f0 = 1.0f - f1;
      const float xLY = lerp(xLM, vtxB.coord.x, f0, f1);
      const float xRY = lerp(xRM, vtxB.coord.x, f0, f1);
      const float dX = xRY - xLY;
      const int xL = rnd(xLY), xR = rnd(xRY);
      float zL, zR;
      if (DEPTH_MODE > NoDepth) {
        zL = lerp(zLM, vtxB.coord.z, f0, f1);
        zR = lerp(zRM, vtxB.coord.z, f0, f1);
      }
      Vec4f colorL, colorR;
      if (SMOOTH) {
        colorL = lerp(colorLM, colorB, f0, f1);
        colorR = lerp(colorRM, colorB, f0, f1);
      }
      Vec2f texCoordL, texCoordR;
      if (TEX) {
        texCoordL = lerp(texCoordLM, vtxB.texCoord, f0, f1);
        texCoordR = lerp(texCoordRM, vtxB.texCoord, f0, f1);
      }
      const size_t i = y * _width;
      for (int x = std::max(xL, 0), xE = std::min(xR, (int)_width);
        x < xE; ++x) {
        const size_t iX = i + x;
        float f1, f0;
        if (DEPTH_MODE > NoDepth || SMOOTH || TEX) {
          f1 = (x - xL) / dX; f0 = 1.0f - f1;
        }
        if (DEPTH_MODE > NoDepth) {
          const float z = lerp(zL, zR, f0, f1);
          if (DEPTH_MODE == DepthCheckAndWrite
            && z >= _fb.depth[iX]) continue;
          _fb.depth[iX] = z;
        }
        if (SMOOTH) {
          color = f0 * colorL + f1 * colorR;
        }
        if (TEX) {
          const Vec2f texCoord = f0 * texCoordL + f1 * texCoordR;
          rgba = color * tex[texCoord];
        } else rgba = color * (uint32)0xffffffff;
        if (BLEND) {
          const float f1 = ((rgba >> 24) & 0xff) * 1.0f / 255;
          const float f0 = 1.0f - f1;
          const Vec4f blendFg(f1, f1, f1, f1), blendBg(f0, f0, f0, f0);
          rgba = blendFg * rgba + blendBg * _fb.rgba[iX];
        } else rgba |= 0xff000000;
        _fb.rgba[iX] = rgba;
      }
    }
  }
}
