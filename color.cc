#include <array>

#include "color.h"

void colorToAlpha(
  uint width, uint height, uint32 img[], const Vec3f &pivot, float exp)
{
  const uint32 *end = img + width * height;
  for (; img != end; ++img) {
    const Vec3f color = rgbaToColor(*img).xyz();
    const float d = length(color - pivot);
    const float alpha = 1.0f - pow(cos(d * (float)Pi), exp);
    (*img &= 0x00ffffff);
    (*img |= (uint32)(0xff000000 * alpha) & (uint32)0xff000000);
  }
}

void mergeColors(
  uint width, uint height,
  uint32 img[], const uint32 img0[], const uint32 img1[],
  float f0, float f1)
{
  const uint32 *end = img + width * height;
  for (; img != end; ++img, ++img0, ++img1) {
    *img = colorToRGBA(
      lerp(rgbaToColor(*img0), rgbaToColor(*img1), f0, f1));
  }
}