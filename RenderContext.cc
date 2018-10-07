#include <algorithm>

#include "Plane.h"
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
  _nVtcs(0)
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
  assert(_nVtcs < 3);
  { Vertex &vtx = _vtcs[_nVtcs];
    const Mat4x4f matMVP = _matProj * _matView * _matModel;
    vtx.coord = transformPoint(matMVP, coord);
    vtx.normal = transformVec(_matModel, _normal);
    vtx.color = _color; vtx.texCoord = _texCoord;
  }
  if (++_nVtcs == 3) {
    uint nVtcs = 3; _nVtcs = 0;
    // face-culling / light correction
    Vec3f light = _light;
    // determine face normal
    const Vec3f normal
      = cross(
        _vtcs[1].coord - _vtcs[0].coord,
        _vtcs[1].coord - _vtcs[2].coord);
    if (normal.z > 0) { // view at back of face 
      if (!isEnabled(BackSide)) return;
      light = -_light;
    } else { // view at front of face
      if (!isEnabled(FrontSide)) return;
    }
    // lighting
    if (isEnabled(Lighting)) {
      _vtcs[0].color
        = lighting(_vtcs[0].color, _vtcs[0].normal, light, _ambient);
      _vtcs[1].color
        = lighting(_vtcs[1].color, _vtcs[1].normal, light, _ambient);
      _vtcs[2].color
        = lighting(_vtcs[2].color, _vtcs[2].normal, light, _ambient);
    }
    { // clipping
      static const Planef clipPlanes[] = {
        Planef(Vec3f(1.0f, 0.0f, 0.0f), 1.0f),
        Planef(Vec3f(-1.0f, 0.0f, 0.0f), 1.0f),
        Planef(Vec3f(0.0f, 1.0f, 0.0f), 1.0f),
        Planef(Vec3f(0.0f, -1.0f, 0.0f), 1.0f),
        Planef(Vec3f(0.0f, 0.0f, 1.0f), 1.0f),
        Planef(Vec3f(0.0f, 0.0f, -1.0f), 1.0f)
      };
      for (const Planef &clipPlane : clipPlanes) {
        uint nVtcsNew = nVtcs;
        for (uint iVtx = 0; iVtx < nVtcs;) {
          switch (clipTri(clipPlane, iVtx, nVtcsNew)) {
            case 0: // triangle outside
              if (nVtcsNew > nVtcs) {
                _vtcs[iVtx + 0] = _vtcs[nVtcsNew - 3];
                _vtcs[iVtx + 1] = _vtcs[nVtcsNew - 2];
                _vtcs[iVtx + 2] = _vtcs[nVtcsNew - 1];
                iVtx += 3;
              } else {
                _vtcs[iVtx + 0] = _vtcs[nVtcs - 3];
                _vtcs[iVtx + 1] = _vtcs[nVtcs - 2];
                _vtcs[iVtx + 2] = _vtcs[nVtcs - 1];
                nVtcs -= 3;
              }
              nVtcsNew -= 3;
              break;
            case 1: // triangle inside
              iVtx += 3;
              break;
            case 2: // triangle split
              iVtx += 3;
              nVtcsNew += 3;
              break;
            default: assert(("unreachable", false));
          }
        }
        if ((nVtcs = nVtcsNew) == 0) break; // early out
      }
    }
    // transform coordinates into screen space
    for (uint iVtx = 0; iVtx < nVtcs; ++iVtx) {
      Vertex &vtx = _vtcs[iVtx];
      vtx.coord = transformPoint(_matScreen, vtx.coord);
    }
    // call rasterize
    typedef void(RenderContext::*Rasterize)(uint);
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
    (this->*rasterizes[i])(nVtcs);
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

RenderContext::Vertex RenderContext::lerpVtx(
  const Vertex &vtx0, const Vertex &vtx1, float f1)
{
  const float f0 = 1.0f - f1;
  return Vertex(
    lerp(vtx0.coord, vtx1.coord, f0, f1),
    Vec3f(Null), // normals not used in this specific case
    isEnabled(Smooth)
    ? lerp(vtx0.color, vtx1.color, f0, f1)
    : vtx0.color,
    lerp(vtx0.texCoord, vtx1.texCoord, f0, f1));
}

uint RenderContext::clipTri(const Planef &plane, uint iVtx0, uint iVtx3)
{
  const float d0 = -getSignDist(plane, _vtcs[iVtx0 + 0].coord);
  const float d1 = -getSignDist(plane, _vtcs[iVtx0 + 1].coord);
  const float d2 = -getSignDist(plane, _vtcs[iVtx0 + 2].coord);
  switch ((d0 >= 0.0f) * 1 | (d1 >= 0.0f) * 2 | (d2 >= 0.0f) * 4) {
    // all vertices outside:
    case 0: return 0;
    // cases with one vertex inside:
    case 1:
      _vtcs[iVtx0 + 1]
        = lerpVtx(_vtcs[iVtx0 + 0], _vtcs[iVtx0 + 1], d0 / (d0 - d1));
      _vtcs[iVtx0 + 2]
        = lerpVtx(_vtcs[iVtx0 + 0], _vtcs[iVtx0 + 2], d0 / (d0 - d2));
      return 1;
    case 2:
      _vtcs[iVtx0 + 0]
        = lerpVtx(_vtcs[iVtx0 + 1], _vtcs[iVtx0 + 0], d1 / (d1 - d0));
      _vtcs[iVtx0 + 2]
        = lerpVtx(_vtcs[iVtx0 + 1], _vtcs[iVtx0 + 2], d1 / (d1 - d2));
      return 1;
    case 4:
      _vtcs[iVtx0 + 0]
        = lerpVtx(_vtcs[iVtx0 + 2], _vtcs[iVtx0 + 0], d2 / (d2 - d0));
      _vtcs[iVtx0 + 1]
        = lerpVtx(_vtcs[iVtx0 + 2], _vtcs[iVtx0 + 1], d2 / (d2 - d1));
      return 1;
    // cases with two vertices inside:
    case 1 | 2:
      _vtcs[iVtx3 + 0]
        = lerpVtx(_vtcs[iVtx0 + 2], _vtcs[iVtx0 + 0], d2 / (d2 - d0));
      _vtcs[iVtx3 + 1] =  _vtcs[iVtx0 + 1];
      _vtcs[iVtx3 + 2]
        = lerpVtx(_vtcs[iVtx0 + 2], _vtcs[iVtx0 + 1], d2 / (d2 - d1));
      _vtcs[iVtx0 + 2] = _vtcs[iVtx3 + 0];
      return 2;
    case 1 | 4:
      _vtcs[iVtx3 + 0] = _vtcs[iVtx0 + 0];
      _vtcs[iVtx3 + 1]
        = lerpVtx(_vtcs[iVtx0 + 1], _vtcs[iVtx0 + 0], d1 / (d1 - d0));
      _vtcs[iVtx3 + 2]
        = lerpVtx(_vtcs[iVtx0 + 1], _vtcs[iVtx0 + 2], d1 / (d1 - d2));
      _vtcs[iVtx0 + 1] = _vtcs[iVtx3 + 2];
      return 2;
    case 2 | 4:
      _vtcs[iVtx3 + 0]
        = lerpVtx(_vtcs[iVtx0 + 0], _vtcs[iVtx0 + 2], d0 / (d0 - d2));
      _vtcs[iVtx3 + 1]
        = lerpVtx(_vtcs[iVtx0 + 0], _vtcs[iVtx0 + 1], d0 / (d0 - d1));
      _vtcs[iVtx3 + 2] = _vtcs[iVtx0 + 2];
      _vtcs[iVtx0 + 0] = _vtcs[iVtx3 + 1];
      return 2;
    // all vertices inside:
    case 1 | 2 | 4: return 1;
    // anything else is illegal
    default: assert(("unreachable", false));
  }
  return (uint)-1;
}

template <
  RenderContext::DepthMode DEPTH_MODE,
  bool SMOOTH,
  bool BLEND,
  bool TEX>
void RenderContext::rasterize(uint nVtcs)
{
  Vec4f color = _vtcs[0].color;
  uint32 rgba = color * (uint32)0xffffffff;
  for (uint iVtx = 0; iVtx < nVtcs; iVtx += 3) {
    // sort vertices by y coordinates
    uint iVtcs[3] = { iVtx + 0, iVtx + 1, iVtx + 2 };
    if (_vtcs[iVtcs[0]].coord.y > _vtcs[iVtcs[1]].coord.y) {
      std::swap(iVtcs[0], iVtcs[1]);
    }
    if (_vtcs[iVtcs[1]].coord.y > _vtcs[iVtcs[2]].coord.y) {
      std::swap(iVtcs[1], iVtcs[2]);
    }
    if (_vtcs[iVtcs[0]].coord.y > _vtcs[iVtcs[1]].coord.y) {
      std::swap(iVtcs[0], iVtcs[1]);
    }
    // cut triangle in upper and lower part
    const Vertex &vtxT = _vtcs[iVtcs[0]];
    const Vertex &vtxM = _vtcs[iVtcs[1]];
    const Vertex &vtxB = _vtcs[iVtcs[2]];
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
}
