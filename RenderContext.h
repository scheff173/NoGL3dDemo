#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <cstdint>
#include <functional>
#include <vector>

#include "linmath.h"
#include "Texture.h"
#include "util.h"

/// @todo double sided
/// @todo face culling
/// @todo clip space culling

class RenderContext {
  public:
    // limits
    enum {
      MaxModelStack = 32
    };
    // modes
    enum Mode {
      FrontSide,
      BackSide,
      DepthBuffer,
      DepthTest,
      Smooth,
      Blending,
      Texturing,
      Lighting,
      NModes
    };
  private:
    // depth mode
    enum DepthMode {
      NoDepth,
      DepthWrite,
      DepthCheckAndWrite
    };
    // vertex
    struct Vertex {
      Vec3f coord;
      Vec3f normal;
      Vec4f color;
      Vec2f texCoord;
    };

  private:
    uint _width, _height;
    uint32 _rgbaClear;
    float _depthClear;
    struct FrameBuffer {
      std::vector<uint32> rgba; // frame buffer for colors
      std::vector<float> depth; // frame buffer for depth values
      FrameBuffer(uint size, uint32 rgba, float depth):
        rgba(size, rgba), depth(size, depth)
      { }
    } _fb;
    Mat4x4f _matScreen;
    Mat4x4f _matProj;
    Mat4x4f _matView;
    Mat4x4f _matCam;
    Mat4x4f _matModel;
    Vec3f _normal;
    Vec4f _color;
    Vec2f _texCoord;
    Vec3f _light;
    float _ambient; // in [0, 1]
    uint _mode;
    uint _iTex;
    bool _vtxColor, _vtxNormal;
    std::vector<Texture> _tex;
    Vertex _vtcs[3];
    uint _iVtx;
    std::function<void(RenderContext&)> _cbRender;

  public:
    // constructor.
    RenderContext(uint width, uint height);
    // destructor.
    ~RenderContext() = default;

    uint getViewportWidth() const { return _width; }
    uint getViewportHeight() const { return _height; }
    void setViewport(uint width, uint height);

    Mat4x4f& getProjMat() { return _matProj; }
    const Mat4x4f& getViewMat() { return _matView; }
    void setViewMat(const Mat4x4f &mat);
    const Mat4x4f& getCamMat() { return _matCam; }
    void setCamMat(const Mat4x4f &mat);
    Mat4x4f& getModelMat() { return _matModel; }

    bool isEnabled(Mode mode) const { return (_mode & (1 << mode)) != 0; }
    void enable(Mode mode, bool enable = true);
    void disable(Mode mode) { enable(mode, false); }

    float getAmbient() const { return _ambient; }
    void setAmbient(float ambient)
    {
      _ambient = clamp(ambient, 0.0f, 1.0f);
    }

    const Vec3f& getNormal() const { return _normal; }
    void setNormal(const Vec3f &value)
    {
      _normal = value;
      _vtxNormal = _iVtx != 0;
    }

    const Vec4f& getColor() const { return _color; }
    void setColor(const Vec4f &value)
    {
      _color = value;
      _vtxColor = _iVtx != 0;
    }

    const Vec2f& getTexCoord() const { return _texCoord; }
    void setTexCoord(const Vec2f &value) { _texCoord = value; }

    void drawVertex(const Vec3f &coord);

    uint loadTex(uint width, uint height, const uint32 img[]);
    uint getTex() const;
    uint setTex(uint i);

    uint getFBI(uint y) const { return y * _width; }
    uint getFBI(uint x, uint y) const { return y * _width + x; }

    void setClearColor(const Vec4f &color);
    void clear(bool rgba, bool depth);

    void setRenderCallback(std::function<void(RenderContext&)> &&cbRender)
    {
      _cbRender = cbRender;
    }

    void render() { _cbRender(*this); }

    const uint32* getRGBA() const { return _fb.rgba.data(); }

  private:

    template <
      DepthMode DEPTH_MODE,
      bool SMOOTH,
      bool BLEND,
      bool TEX>
    void rasterize(const Vertex (&vtcs)[3]);
};

#endif // RENDER_CONTEXT_H