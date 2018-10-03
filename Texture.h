#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>

#include "util.h"
#include "linmath.h"

class Texture {
  private:
    uint _width, _height;
    std::vector<uint32> _texel;
    uint _mU, _mV;

  public:
    Texture(): _width(0), _height(0) { }
    Texture(uint width, uint height, const uint32 data[]):
      _width(width), _height(height), _texel(data, data + width * height),
      _mU(width - 1), _mV(height - 1)
    {
      assert(isPowerOf2(_width) && isPowerOf2(_height));
    }
    ~Texture() = default;
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture &&tex):
      _width(tex._width), _height(tex._height),
      _texel(std::move(tex._texel)),
      _mU(tex._mU), _mV(tex._mV)
    { }
    Texture& operator=(Texture &&tex)
    {
      _width = tex._width; _height = tex._height;
      _texel = std::move(tex._texel);
      _mU = tex._mU; _mV = tex._mV;
      tex._width = tex._height = 0;
      return *this;
    }
    bool empty() const { return _width * _height == 0; }
    bool load(uint width, uint height, const uint32 data[])
    {
      if (!(isPowerOf2(width) && isPowerOf2(height))) return false;
      _width = width; _height = height;
      const size_t size = width * height;
      _texel.assign(data, data + size);
      _mU = _width - 1; _mV = _height - 1;
      return true;
    }
    uint32 operator[](const Vec2f &coord) const
    {
      assert(_width != 0 && _height != 0);
#if 1 // should work
      const float u = coord.x < 0.0f
        ? 1.0f - coord.x - (int)coord.x
        : coord.x - (int)coord.x;
      const float v = coord.y < 0.0f
        ? 1.0f - coord.y - (int)coord.y
        : coord.y - (int)coord.y;
      return _texel[(size_t)(v * _mV) * _width + (size_t)(u * _mU)];
#else // to explore
      return _texel[(size_t)((uint)(v * _height) & _mv) * _width
        + ((uint)(u * _width) & _mu];
#endif // 1
    }
};

#endif // TEXTURE_H
