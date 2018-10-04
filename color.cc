#include <array>

#include "color.h"

void colorToAlpha(
  uint width, uint height, uint32 img[], const Vec3f &pivot, float exp)
{
  // maximum distance is length(Vec3f(1, 1, 1) - Vec3f(0, 0, 0))
  const float dMax = sqrt(3.0f);
  // scale d to range of [0, 90] degree
  const float normD = 0.5f * (float)Pi / dMax;
  // process any pixel of image
  const uint32 *end = img + width * height;
  for (; img != end; ++img) {
    const Vec3f color = rgbaToColor(*img).xyz();
    const float d = length(color - pivot);
    const float alpha = 1.0f - pow(cos(d * normD), exp);
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