#ifndef PLANE_H
#define PLANE_H

#include "linmath.h"

template <typename VALUE>
class PlaneT {
  public:
    typedef VALUE Value;
  private:
    Vec3T<VALUE> _normal;
    float _d;
  public:
    // default constructor (leaving elements uninitialized)
    PlaneT() { }
    PlaneT(const Vec3T<VALUE> &normal, VALUE d):
      _normal(normalize(normal)), _d(d)
    { }
    PlaneT(const PlaneT&) = default;
    PlaneT& operator=(const PlaneT&) = default;
    ~PlaneT() = default;
  public:
    const Vec3T<VALUE>& getNormal() const { return _normal; }
    VALUE getD() const { return _d; }
};

typedef PlaneT<double> Plane;
typedef PlaneT<float> Planef;

/** computes signed distance of a point from plane.
 *
 * Point over plane means the half space into this normal is directing.
 *
 * @param pnt point
 * @return \>= 0 ... distance (point over plane)\n
 *         \< 0 ... -distance (point under plane)
 */
template <typename VALUE>
inline VALUE getSignDist(
  const PlaneT<VALUE> &plane, const Vec3T<VALUE> &pnt)
{
  return dot(pnt - (plane.getD() * plane.getNormal()), plane.getNormal());
}

#endif // PLANE_H
