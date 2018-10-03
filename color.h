#ifndef COLOR_H
#define COLOR_H

#include "util.h"
#include "linmath.h"

inline uint32 colorToRGBA(const Vec4f &color)
{
  return ((uint32)(0xff000000 * color.w) & (uint32)0xff000000)
    | ((uint32)(0x00ff0000 * color.z) & (uint32)0x00ff0000)
    | ((uint32)(0x0000ff00 * color.y) & (uint32)0x0000ff00)
    | ((uint32)(0x000000ff * color.x) & (uint32)0x000000ff);
}

inline Vec4f rgbaToColor(const uint32 rgba)
{
  return Vec4f(
    ((rgba >> 0) & 0xff) * (1.0f / 255),
    ((rgba >> 8) & 0xff) * (1.0f / 255),
    ((rgba >> 16) & 0xff) * (1.0f / 255),
    ((rgba >> 24) & 0xff) * (1.0f / 255));
}

void colorToAlpha(
  uint width, uint height, uint32 img[], const Vec3f &pivot, float exp);

void mergeColors(
  uint width, uint height,
  uint32 img[], const uint32 img0[], const uint32 img1[],
  float f0, float f1);

#endif // COLOR_H
