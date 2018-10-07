/** @file
 * functions to work with RGBA colors
 */

#ifndef COLOR_H
#define COLOR_H

// own header:
#include "util.h"
#include "linmath.h"

/** converts a color from 4 vector to RGBA 32-bit values.
 *
 * @param color color as 4 vector
 * @return color as RGBA 32-bit value
 */
inline uint32 colorToRGBA(const Vec4f &color)
{
  return ((uint32)(0xff000000 * color.w) & (uint32)0xff000000)
    | ((uint32)(0x00ff0000 * color.z) & (uint32)0x00ff0000)
    | ((uint32)(0x0000ff00 * color.y) & (uint32)0x0000ff00)
    | ((uint32)(0x000000ff * color.x) & (uint32)0x000000ff);
}

/** converts a color from RGBA 32-bit values to 4 vector.
 *
 * @param rgba as RGBA 32-bit value
 * @return color as 4 vector
 */
inline Vec4f rgbaToColor(const uint32 rgba)
{
  return Vec4f(
    ((rgba >> 0) & 0xff) * (1.0f / 255),
    ((rgba >> 8) & 0xff) * (1.0f / 255),
    ((rgba >> 16) & 0xff) * (1.0f / 255),
    ((rgba >> 24) & 0xff) * (1.0f / 255));
}

/** converts colors to alpha values.
 *
 * The alpha values are computed by 1 - cos(d)^exp
 * where d is the distance between color and pivot color
 * normalized to the range [0, 90] deg.
 *
 * @param width width of image
 * @param height height of image the
 * @param img color values of image (alpha values will be updated)
 *        (from top/left to bottom/right corner)\n
 *        The image @a img has to provide at least width * height RGBA
 *        values.
 *        Thereby, in each element R (red) has to be stored in the
 *        least significant byte, A (alpha) in the most significant.
 * @param pivot the pivot color
 * @param exp the exponent for cos()
 */
void colorToAlpha(
  uint width, uint height, uint32 img[], const Vec3f &pivot, float exp);

/** merges two images.
 *
 * @param width width of image
 * @param height height of image the
 * @param img result image (might be @a img0 or @a img1)
 * @param img0 first image
 * @param img1 second image
 * @param f0 factor for @a img0 [0, 1]
 * @param f1 factor for @a img1 [0, 1 - @a f0]
 */
void mergeColors(
  uint width, uint height,
  uint32 img[], const uint32 img0[], const uint32 img1[],
  float f0, float f1);

#endif // COLOR_H
