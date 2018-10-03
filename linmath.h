#ifndef LIN_MATH_H
#define LIN_MATH_H

#include <iostream>
#include <cassert>
#include <cmath>

extern const double Pi;

template <typename VALUE>
inline VALUE degToRad(VALUE angle)
{
  return (VALUE)Pi * angle / (VALUE)180;
}

template <typename VALUE>
inline VALUE radToDeg(VALUE angle)
{
  return (VALUE)180 * angle / (VALUE)Pi;
}

enum ArgNull { Null };

template <typename VALUE>
struct Vec2T {
  typedef VALUE Value;
  Value x, y;
  // default constructor (leaving elements uninitialized)
  Vec2T() { }
  Vec2T(ArgNull): x((Value)0), y((Value)0) { }
  Vec2T(Value x, Value y): x(x), y(y) { }
};

template <typename VALUE>
Vec2T<VALUE> operator+(const Vec2T<VALUE> &v) { return v; }

template <typename VALUE>
Vec2T<VALUE> operator-(const Vec2T<VALUE> &v)
{
  return Vec2T<VALUE>(-v.x, -v.y);
}

template <typename VALUE>
Vec2T<VALUE> operator+(const Vec2T<VALUE> &v1, const Vec2T<VALUE> &v2)
{
  return Vec2T<VALUE>(v1.x + v2.x, v1.y + v2.y);
}

template <typename VALUE>
Vec2T<VALUE> operator-(const Vec2T<VALUE> &v1, const Vec2T<VALUE> &v2)
{
  return Vec2T<VALUE>(v1.x - v2.x, v1.y - v2.y);
}

template <typename VALUE>
Vec2T<VALUE> operator*(const Vec2T<VALUE> &v, VALUE s)
{
  return Vec2T<VALUE>(v.x * s, v.y * s);
}

template <typename VALUE>
Vec2T<VALUE> operator*(VALUE s, const Vec2T<VALUE> &v) { return v * s; }

template <typename VALUE>
std::ostream& operator<<(std::ostream &out, const Vec2T<VALUE> &v)
{
  return out << "( " << v.x << ", " << v.y << " )";
}

template <typename VALUE>
VALUE manhattan(const Vec2T<VALUE> &vec)
{
  return vec.x * vec.x + vec.y * vec.y;
}

typedef Vec2T<float> Vec2f;
typedef Vec2T<double> Vec2;

template <typename VALUE>
struct Vec3T {
  typedef VALUE Value;
  Value x, y, z;
  // default constructor (leaving elements uninitialized)
  Vec3T() { }
  Vec3T(ArgNull): x((Value)0), y((Value)0), z((Value)0) { }
  Vec3T(Value x, Value y, Value z): x(x), y(y), z(z) { }
  Vec3T(const Vec2T<Value> &xy, Value z): x(xy.x), y(xy.y), z(z) { }
  explicit operator Vec2T<Value>() const { return Vec2T<Value>(x, y); }
  const Vec2f xy() const { return Vec2f(x, y); }
  const Vec2f xz() const { return Vec2f(x, z); }
  const Vec2f yz() const { return Vec2f(y, z); }
};

template <typename VALUE>
Vec3T<VALUE> operator+(const Vec3T<VALUE> &v) { return v; }

template <typename VALUE>
Vec3T<VALUE> operator-(const Vec3T<VALUE> &v)
{
  return Vec3T<VALUE>(-v.x, -v.y, -v.z);
}

template <typename VALUE>
Vec3T<VALUE> operator+(const Vec3T<VALUE> &v1, const Vec3T<VALUE> &v2)
{
  return Vec3T<VALUE>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template <typename VALUE>
Vec3T<VALUE> operator-(const Vec3T<VALUE> &v1, const Vec3T<VALUE> &v2)
{
  return Vec3T<VALUE>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template <typename VALUE>
Vec3T<VALUE> operator*(const Vec3T<VALUE> &v, VALUE s)
{
  return Vec3T<VALUE>(v.x * s, v.y * s, v.z * s);
}

template <typename VALUE>
Vec3T<VALUE> operator*(VALUE s, const Vec3T<VALUE> &v) { return v * s; }

template <typename VALUE>
std::ostream& operator<<(std::ostream &out, const Vec3T<VALUE> &v)
{
  return out << "( " << v.x << ", " << v.y << ", " << v.z << " )";
}

template <typename VALUE>
VALUE manhattan(const Vec3T<VALUE> &vec)
{
  return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

template <typename VALUE>
VALUE dot(const Vec3T<VALUE> &v1, const Vec3T<VALUE> &v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename VALUE>
Vec3T<VALUE> cross(const Vec3T<VALUE> &v1, const Vec3T<VALUE> &v2)
{
  return Vec3T<VALUE>(
    v1.y * v2.z - v1.z * v2.y,
    v1.z * v2.x - v1.x * v2.z,
    v1.x * v2.y - v1.y * v2.x);
}

typedef Vec3T<float> Vec3f;
typedef Vec3T<double> Vec3;

template <typename VALUE>
struct Vec4T {
  typedef VALUE Value;
  Value x, y, z, w;
  // default constructor (leaving elements uninitialized)
  Vec4T() { }
  Vec4T(ArgNull): x((Value)0), y((Value)0), z((Value)0), w((Value)0) { }
  Vec4T(Value x, Value y, Value z, Value w): x(x), y(y), z(z), w(w) { }
  Vec4T(const Vec2T<Value> &xy, Value z, Value w):
    x(xy.x), y(xy.y), z(z), w(w)
  { }
  Vec4T(const Vec3T<Value> &xyz, Value w):
    x(xyz.x), y(xyz.y), z(xyz.z), w(w)
  { }
  explicit operator Vec2T<Value>() const { return Vec2T<Value>(x, y); }
  explicit operator Vec3T<Value>() const { return Vec3T<Value>(x, y, z); }
  const Vec2f xy() const { return Vec2f(x, y); }
  const Vec2f xz() const { return Vec2f(x, z); }
  const Vec2f yz() const { return Vec2f(y, z); }
  const Vec3f xyz() const { return Vec3f(x, y, z); }
};

template <typename VALUE>
Vec4T<VALUE> operator+(const Vec4T<VALUE> &v) { return v; }

template <typename VALUE>
Vec4T<VALUE> operator-(const Vec4T<VALUE> &v)
{
  return Vec4T<VALUE>(-v.x, -v.y, -v.z, -v.w);
}

template <typename VALUE>
Vec4T<VALUE> operator+(const Vec4T<VALUE> &v1, const Vec4T<VALUE> &v2)
{
  return Vec4T<VALUE>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

template <typename VALUE>
Vec4T<VALUE> operator-(const Vec4T<VALUE> &v1, const Vec4T<VALUE> &v2)
{
  return Vec4T<VALUE>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

template <typename VALUE>
Vec4T<VALUE> operator*(const Vec4T<VALUE> &v, VALUE s)
{
  return Vec4T<VALUE>(v.x * s, v.y * s, v.z * s, v.w * s);
}

template <typename VALUE>
Vec4T<VALUE> operator*(VALUE s, const Vec4T<VALUE> &v) { return v * s; }

template <typename VALUE>
std::ostream& operator<<(std::ostream &out, const Vec4T<VALUE> &v)
{
  return out << "( " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << " )";
}

template <typename VALUE>
VALUE manhattan(const Vec4T<VALUE> &vec)
{
  return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
}

typedef Vec4T<float> Vec4f;
typedef Vec4T<double> Vec4;

enum ArgInitIdent { InitIdent };
enum ArgInitTrans { InitTrans };
enum ArgInitRot { InitRot };
enum ArgInitRotX { InitRotX };
enum ArgInitRotY { InitRotY };
enum ArgInitRotZ { InitRotZ };
enum ArgInitScale { InitScale };
enum ArgInitPersp { InitPersp };

template <typename VALUE>
struct Mat4x4T {
  union {
    VALUE comp[4 * 4];
    struct {
      VALUE _00, _01, _02, _03;
      VALUE _10, _11, _12, _13;
      VALUE _20, _21, _22, _23;
      VALUE _30, _31, _32, _33;
    };
  };

  // default constructor (leaving elements uninitialized)
  Mat4x4T() { }
  // constructor to build a matrix by elements
  Mat4x4T(
    VALUE _00, VALUE _01, VALUE _02, VALUE _03,
    VALUE _10, VALUE _11, VALUE _12, VALUE _13,
    VALUE _20, VALUE _21, VALUE _22, VALUE _23,
    VALUE _30, VALUE _31, VALUE _32, VALUE _33):
    _00(_00), _01(_01), _02(_02), _03(_03),
    _10(_10), _11(_11), _12(_12), _13(_13),
    _20(_20), _21(_21), _22(_22), _23(_23),
    _30(_30), _31(_31), _32(_32), _33(_33)
  { }
  // constructor to build an identity matrix
  Mat4x4T(ArgInitIdent):
    _00((VALUE)1), _01((VALUE)0), _02((VALUE)0), _03((VALUE)0),
    _10((VALUE)0), _11((VALUE)1), _12((VALUE)0), _13((VALUE)0),
    _20((VALUE)0), _21((VALUE)0), _22((VALUE)1), _23((VALUE)0),
    _30((VALUE)0), _31((VALUE)0), _32((VALUE)0), _33((VALUE)1)
  { }
  // constructor to build a matrix for translation
  Mat4x4T(ArgInitTrans, const Vec3T<VALUE> &t):
    _00((VALUE)1), _01((VALUE)0), _02((VALUE)0), _03((VALUE)t.x),
    _10((VALUE)0), _11((VALUE)1), _12((VALUE)0), _13((VALUE)t.y),
    _20((VALUE)0), _21((VALUE)0), _22((VALUE)1), _23((VALUE)t.z),
    _30((VALUE)0), _31((VALUE)0), _32((VALUE)0), _33((VALUE)1)
  { }
  // constructor to build a matrix for rotation about axis
  Mat4x4T(ArgInitRot, const Vec3T<VALUE> &axis, VALUE angle):
    _03((VALUE)0), _13((VALUE)0), _23((VALUE)0),
    _30((VALUE)0), _31((VALUE)0), _32((VALUE)0), _33((VALUE)1)
  {
    //axis.normalize();
    const VALUE sinAngle = sin(angle), cosAngle = cos(angle);
    const VALUE xx = axis.x * axis.x, xy = axis.x * axis.y;
    const VALUE xz = axis.x * axis.z, yy = axis.y * axis.y;
    const VALUE yz = axis.y * axis.z, zz = axis.z * axis.z;
    _00 = xx + cosAngle * ((VALUE)1 - xx) /* + sinAngle * 0 */;
    _01 = xy - cosAngle * xy - sinAngle * axis.z;
    _02 = xz - cosAngle * xz + sinAngle * axis.y;
    _10 = xy - cosAngle * xy + sinAngle * axis.z;
    _11 = yy + cosAngle * ((VALUE)1 - yy) /* + sinAngle * 0 */;
    _12 = yz - cosAngle * yz - sinAngle * axis.x;
    _20 = xz - cosAngle * xz - sinAngle * axis.y;
    _21 = yz - cosAngle * yz + sinAngle * axis.x;
    _22 = zz + cosAngle * ((VALUE)1 - zz) /* + sinAngle * 0 */;
  }
  // constructor to build a matrix for rotation about x axis
  Mat4x4T(ArgInitRotX, VALUE angle):
    _00((VALUE)1), _01((VALUE)0),    _02((VALUE)0),   _03((VALUE)0),
    _10((VALUE)0), _11(cos(angle)),  _12(-sin(angle)), _13((VALUE)0),
    _20((VALUE)0), _21(sin(angle)), _22(cos(angle)), _23((VALUE)0),
    _30((VALUE)0), _31((VALUE)0),    _32((VALUE)0),   _33((VALUE)1)
  { }
  // constructor to build a matrix for rotation about y axis
  Mat4x4T(ArgInitRotY, VALUE angle):
    _00(cos(angle)), _01((VALUE)0), _02(sin(angle)), _03((VALUE)0),
    _10((VALUE)0),   _11((VALUE)1), _12((VALUE)0),    _13((VALUE)0),
    _20(-sin(angle)), _21((VALUE)0), _22(cos(angle)),  _23((VALUE)0),
    _30((VALUE)0), _31((VALUE)0),    _32((VALUE)0),   _33((VALUE)1)
  { }
  // constructor to build a matrix for rotation about z axis
  Mat4x4T(ArgInitRotZ, VALUE angle):
    _00(cos(angle)),  _01(-sin(angle)), _02((VALUE)0), _03((VALUE)0),
    _10(sin(angle)), _11(cos(angle)), _12((VALUE)0), _13((VALUE)0),
    _20((VALUE)0),    _21((VALUE)0),   _22((VALUE)1), _23((VALUE)0),
    _30((VALUE)0),    _31((VALUE)0),   _32((VALUE)0), _33((VALUE)1)
  { }
  // constructor to build a matrix for scaling
  Mat4x4T(ArgInitScale, VALUE sx, VALUE sy, VALUE sz):
    _00((VALUE)sx), _01((VALUE)0),  _02((VALUE)0),  _03((VALUE)0),
    _10((VALUE)0),  _11((VALUE)sy), _12((VALUE)0),  _13((VALUE)0),
    _20((VALUE)0),  _21((VALUE)0),  _22((VALUE)sz), _23((VALUE)0),
    _30((VALUE)0),  _31((VALUE)0),  _32((VALUE)0),  _33((VALUE)1)
  { }
  /* constructor to build a matrix for perspective view.
   *
   * l ... left border
   * r ... right border
   * t ... top border
   * b ... bottom border
   * n ... near clip distance
   * f ... far clip distance
   */
  Mat4x4T(
    ArgInitPersp, VALUE l, VALUE r, VALUE b, VALUE t, VALUE n, VALUE f):
    _00(((VALUE)2 * n) / (r - l)),
    _01((VALUE)0),
    _02((r + l) / (r - l)),
    _03((VALUE)0),
    _10((VALUE)0),
    _11(((VALUE)2 * n) / (t - b)),
    _12((t + b) / (t - b)),
    _13((VALUE)0),
    _20((VALUE)0),
    _21((VALUE)0),
    _22(-(f + n) / (f - n)),
    _23(((VALUE)-2 * f * n) / (f - n)),
    _30((VALUE)0),
    _31((VALUE)0),
    _32((VALUE)-1),
    _33((VALUE)0)
  { }
  // operator to allow access with [][]
  VALUE* operator [] (int i)
  {
    assert(i >= 0 && i < 4);
    return comp + 4 * i;
  }
  // operator to allow access with [][]
  const VALUE* operator [] (int i) const
  {
    assert(i >= 0 && i < 4);
    return comp + 4 * i;
  }
  // multiply matrix with matrix -> matrix
  Mat4x4T operator * (const Mat4x4T &mat) const
  {
    return Mat4x4T(
      _00 * mat._00 + _01 * mat._10 + _02 * mat._20 + _03 * mat._30,
      _00 * mat._01 + _01 * mat._11 + _02 * mat._21 + _03 * mat._31,
      _00 * mat._02 + _01 * mat._12 + _02 * mat._22 + _03 * mat._32,
      _00 * mat._03 + _01 * mat._13 + _02 * mat._23 + _03 * mat._33,
      _10 * mat._00 + _11 * mat._10 + _12 * mat._20 + _13 * mat._30,
      _10 * mat._01 + _11 * mat._11 + _12 * mat._21 + _13 * mat._31,
      _10 * mat._02 + _11 * mat._12 + _12 * mat._22 + _13 * mat._32,
      _10 * mat._03 + _11 * mat._13 + _12 * mat._23 + _13 * mat._33,
      _20 * mat._00 + _21 * mat._10 + _22 * mat._20 + _23 * mat._30,
      _20 * mat._01 + _21 * mat._11 + _22 * mat._21 + _23 * mat._31,
      _20 * mat._02 + _21 * mat._12 + _22 * mat._22 + _23 * mat._32,
      _20 * mat._03 + _21 * mat._13 + _22 * mat._23 + _23 * mat._33,
      _30 * mat._00 + _31 * mat._10 + _32 * mat._20 + _33 * mat._30,
      _30 * mat._01 + _31 * mat._11 + _32 * mat._21 + _33 * mat._31,
      _30 * mat._02 + _31 * mat._12 + _32 * mat._22 + _33 * mat._32,
      _30 * mat._03 + _31 * mat._13 + _32 * mat._23 + _33 * mat._33);
  }
  // multiply matrix with vector -> vector
  Vec4T<VALUE> operator * (const Vec4T<VALUE> &vec) const
  {
    return Vec4T<VALUE>(
      _00 * vec.x + _01 * vec.y + _02 * vec.z + _03 * vec.w,
      _10 * vec.x + _11 * vec.y + _12 * vec.z + _13 * vec.w,
      _20 * vec.x + _21 * vec.y + _22 * vec.z + _23 * vec.w,
      _30 * vec.x + _31 * vec.y + _32 * vec.z + _33 * vec.w);
  }
};

template <typename VALUE>
std::ostream& operator<<(std::ostream &out, const Mat4x4T<VALUE> &m)
{
  return out
    << m._00 << '\t' << m._01 << '\t' << m._02 << '\t' << m._03 << '\n'
    << m._10 << '\t' << m._11 << '\t' << m._12 << '\t' << m._13 << '\n'
    << m._20 << '\t' << m._21 << '\t' << m._22 << '\t' << m._23 << '\n'
    << m._30 << '\t' << m._31 << '\t' << m._32 << '\t' << m._33 << '\n';
}

/* computes determinant of a matrix.
 *
 * det = |M|
 *
 * mat ... the matrix
 */
template <typename VALUE>
VALUE determinant(const Mat4x4T<VALUE> &mat)
{
  /* determinant of 4x4 matrix is computed from
   * sum of m00 * M00 - m01 * M01 + m02 * M02 - m03 * M03
   * M00 ... M03 are the minors of the elements m00 ... m03
   */
  return
    mat._00
      * (mat._11 * (mat._22 * mat._33 - mat._23 * mat._32)
        + mat._12 * (mat._23 * mat._31 - mat._21 * mat._33)
        + mat._13 * (mat._21 * mat._32 - mat._22 * mat._31))
    - mat._01
      * (mat._10 * (mat._22 * mat._33 - mat._23 * mat._32)
        + mat._12 * (mat._23 * mat._30 - mat._20 * mat._33)
        + mat._13 * (mat._20 * mat._32 - mat._22 * mat._30))
    + mat._02
      * (mat._10 * (mat._21 * mat._33 - mat._23 * mat._31)
        + mat._11 * (mat._23 * mat._30 - mat._20 * mat._33)
        + mat._13 * (mat._20 * mat._31 - mat._21 * mat._30))
    - mat._03
      * (mat._10 * (mat._21 * mat._32 - mat._22 * mat._31)
        + mat._11 * (mat._22 * mat._30 - mat._20 * mat._32)
        + mat._12 * (mat._20 * mat._31 - mat._21 * mat._30));
}

/* returns the inverse of a regular matrix.
 *
 * mat matrix to invert
 * eps epsilon for regularity of matrix
 */
template <typename VALUE>
Mat4x4T<VALUE> invert(
  const Mat4x4T<VALUE> &mat, VALUE eps = (VALUE)1E-10)
{
  assert(eps >= (VALUE)0);
  // compute determinant and check that it its unequal to 0
  // (Otherwise, matrix is singular!)
  const VALUE det = determinant(mat);
  if (abs(det) < eps) throw std::domain_error("Singular matrix!");
  // reciproke of determinant
  const VALUE detInvPos = (VALUE)1 / det, detInvNeg = -detInvPos;
  // compute each element by determinant of sub-matrix which is build
  // striking out row and column of pivot element itself
  // BTW, the determinant is multiplied with -1 when sum of row and column
  // index is odd (chess board rule)
  // (This is usually called cofactor of related element.)
  // transpose matrix and multiply with 1/determinant of original matrix
  return Mat4x4T<VALUE>(
    detInvPos
      * (mat._11 * mat._22 * mat._33
        + mat._12 * mat._23 * mat._31
        + mat._13 * mat._21 * mat._32
        - mat._13 * mat._22 * mat._31
        - mat._11 * mat._23 * mat._32
        - mat._12 * mat._21 * mat._33),
    detInvNeg
      * (mat._01 * mat._22 * mat._33
        + mat._02 * mat._23 * mat._31
        + mat._03 * mat._21 * mat._32
        - mat._03 * mat._22 * mat._31
        - mat._01 * mat._23 * mat._32
        - mat._02 * mat._21 * mat._33),
    detInvPos
      * (mat._01 * mat._12 * mat._33
        + mat._02 * mat._13 * mat._31
        + mat._03 * mat._11 * mat._32
        - mat._03 * mat._12 * mat._31
        - mat._01 * mat._13 * mat._32
        - mat._02 * mat._11 * mat._33),
    detInvNeg
      * (mat._01 * mat._12 * mat._23
        + mat._02 * mat._13 * mat._21
        + mat._03 * mat._11 * mat._22
        - mat._03 * mat._12 * mat._21
        - mat._01 * mat._13 * mat._22
        - mat._02 * mat._11 * mat._23),
    detInvNeg
      * (mat._10 * mat._22 * mat._33
        + mat._12 * mat._23 * mat._30
        + mat._13 * mat._20 * mat._32
        - mat._13 * mat._22 * mat._30
        - mat._10 * mat._23 * mat._32
        - mat._12 * mat._20 * mat._33),
    detInvPos
      * (mat._00 * mat._22 * mat._33
        + mat._02 * mat._23 * mat._30
        + mat._03 * mat._20 * mat._32
        - mat._03 * mat._22 * mat._30
        - mat._00 * mat._23 * mat._32
        - mat._02 * mat._20 * mat._33),
    detInvNeg
      * (mat._00 * mat._12 * mat._33
        + mat._02 * mat._13 * mat._30
        + mat._03 * mat._10 * mat._32
        - mat._03 * mat._12 * mat._30
        - mat._00 * mat._13 * mat._32
        - mat._02 * mat._10 * mat._33),
    detInvPos
      * (mat._00 * mat._12 * mat._23
        + mat._02 * mat._13 * mat._20
        + mat._03 * mat._10 * mat._22
        - mat._03 * mat._12 * mat._20
        - mat._00 * mat._13 * mat._22
        - mat._02 * mat._10 * mat._23),
    detInvPos
      * (mat._10 * mat._21 * mat._33
        + mat._11 * mat._23 * mat._30
        + mat._13 * mat._20 * mat._31
        - mat._13 * mat._21 * mat._30
        - mat._10 * mat._23 * mat._31
        - mat._11 * mat._20 * mat._33),
    detInvNeg
      * (mat._00 * mat._21 * mat._33
        + mat._01 * mat._23 * mat._30
        + mat._03 * mat._20 * mat._31
        - mat._03 * mat._21 * mat._30
        - mat._00 * mat._23 * mat._31
        - mat._01 * mat._20 * mat._33),
    detInvPos
      * (mat._00 * mat._11 * mat._33
        + mat._01 * mat._13 * mat._30
        + mat._03 * mat._10 * mat._31
        - mat._03 * mat._11 * mat._30
        - mat._00 * mat._13 * mat._31
        - mat._01 * mat._10 * mat._33),
    detInvNeg
      * (mat._00 * mat._11 * mat._23
        + mat._01 * mat._13 * mat._20
        + mat._03 * mat._10 * mat._21
        - mat._03 * mat._11 * mat._20
        - mat._00 * mat._13 * mat._21
        - mat._01 * mat._10 * mat._23),
    detInvNeg
      * (mat._10 * mat._21 * mat._32
        + mat._11 * mat._22 * mat._30
        + mat._12 * mat._20 * mat._31
        - mat._12 * mat._21 * mat._30
        - mat._10 * mat._22 * mat._31
        - mat._11 * mat._20 * mat._32),
    detInvPos
      * (mat._00 * mat._21 * mat._32
        + mat._01 * mat._22 * mat._30
        + mat._02 * mat._20 * mat._31
        - mat._02 * mat._21 * mat._30
        - mat._00 * mat._22 * mat._31
        - mat._01 * mat._20 * mat._32),
    detInvNeg
      * (mat._00 * mat._11 * mat._32
        + mat._01 * mat._12 * mat._30
        + mat._02 * mat._10 * mat._31
        - mat._02 * mat._11 * mat._30
        - mat._00 * mat._12 * mat._31
        - mat._01 * mat._10 * mat._32),
    detInvPos
      * (mat._00 * mat._11 * mat._22
        + mat._01 * mat._12 * mat._20
        + mat._02 * mat._10 * mat._21
        - mat._02 * mat._11 * mat._20
        - mat._00 * mat._12 * mat._21
        - mat._01 * mat._10 * mat._22));
}

typedef Mat4x4T<float> Mat4x4f;
typedef Mat4x4T<double> Mat4x4;

template <typename VECTOR>
typename VECTOR::Value length(const VECTOR &vec)
{
  return std::sqrt(manhattan(vec));
}

enum ArgNoThrow { NoThrow };

template <typename VECTOR>
VECTOR normalize(const VECTOR &vec)
{
  typedef typename VECTOR::Value Value;
  const Value len = length(vec);
  if (len < (Value)1E-10) throw std::domain_error("Null vector!");
  return vec * ((Value)1 / len);
}

template <typename VECTOR>
VECTOR normalize(const VECTOR &vec, ArgNoThrow)
{
  typedef typename VECTOR::Value Value;
  const Value len = length(vec);
  return len < (Value)1E-10 ? VECTOR(Null) : vec * ((Value)1 / len);
}

template <typename VALUE>
Vec3T<VALUE> transformPoint(
  const Mat4x4T<VALUE> &mat, const Vec3T<VALUE> &pt)
{
  Vec4T<VALUE> pt_ = mat * Vec4T<VALUE>(pt.x, pt.y, pt.z, (VALUE)1);
  return pt_.w != (VALUE)0
    ? Vec3T<VALUE>(pt_.x / pt_.w, pt_.y / pt_.w, pt_.z / pt_.w)
    : Vec3T<VALUE>(pt_.x, pt_.y, pt_.z);
}

template <typename VALUE>
Vec3T<VALUE> transformVec(
  const Mat4x4T<VALUE> &mat, const Vec3T<VALUE> &v)
{
  Vec4T<VALUE> vec = mat * Vec4T<VALUE>(v.x, v.y, v.z, (VALUE)0);
  return Vec3T<VALUE>(vec.x, vec.y, vec.z);
}

/* builds a symmetrical perspective view matrix.
 *
 * fov ... vertical field of view (in rad.)
 * ar ... aspect ratio (view width / view height)
 * dNear ... distance of near clip plane
 * dFar ... distance of far clip plane
 */
template <typename VALUE>
inline Mat4x4T<VALUE> makePersp(
  VALUE fov, VALUE ar, VALUE dNear, VALUE dFar)
{
  const VALUE d = std::tan(fov / 2.0) * 2.0 * dNear;
  return Mat4x4T<VALUE>(InitPersp, -d * ar, d * ar, -d, d, dNear, dFar);
}

// enumeration of rotation axes
enum RotAxis {
  RotX, // rotation about x axis
  RotY, // rotation about y axis
  RotZ // rotation about z axis
};

// enumeration of possible Euler angles
enum EulerAngle {
  RotXYX = RotX + 3 * RotY + 9 * RotX, // 0 + 3 + 0 = 3
  RotXYZ = RotX + 3 * RotY + 9 * RotZ, // 0 + 3 + 18 = 21
  RotXZX = RotX + 3 * RotZ + 9 * RotX, // 0 + 6 + 0 = 6
  RotXZY = RotX + 3 * RotZ + 9 * RotY, // 0 + 6 + 9 = 15
  RotYXY = RotY + 3 * RotX + 9 * RotY, // 1 + 0 + 9 = 10
  RotYXZ = RotY + 3 * RotX + 9 * RotZ, // 1 + 0 + 18 = 19
  RotYZX = RotY + 3 * RotZ + 9 * RotX, // 1 + 6 + 0 = 7
  RotYZY = RotY + 3 * RotZ + 9 * RotY, // 1 + 6 + 9 = 16
  RotZXY = RotZ + 3 * RotX + 9 * RotY, // 2 + 0 + 9 = 11
  RotZXZ = RotZ + 3 * RotX + 9 * RotZ, // 2 + 0 + 18 = 20
  RotZYX = RotZ + 3 * RotY + 9 * RotX, // 2 + 3 + 0 = 5
  RotZYZ = RotZ + 3 * RotY + 9 * RotZ, // 2 + 3 + 18 = 23
  RotHPR = RotZXY, // used in OpenGL Performer
  RotABC = RotZYX // used in German engineering
};

/* decomposes the combined EULER angle type into the corresponding
 * individual EULER angle axis types.
 */
inline void decompose(
  EulerAngle type, RotAxis &axis1, RotAxis &axis2, RotAxis &axis3)
{
  unsigned type_ = (unsigned)type;
  axis1 = (RotAxis)(type_ % 3); type_ /= 3;
  axis2 = (RotAxis)(type_ % 3); type_ /= 3;
  axis3 = (RotAxis)type_;
}

template <typename VALUE>
Mat4x4T<VALUE> makeEuler(
  EulerAngle mode, VALUE rot1, VALUE rot2, VALUE rot3)
{
  RotAxis axis1, axis2, axis3;
  decompose(mode, axis1, axis2, axis3);
  const static VALUE axes[3][3] = {
    { (VALUE)1, (VALUE)0, (VALUE)0 },
    { (VALUE)0, (VALUE)1, (VALUE)0 },
    { (VALUE)0, (VALUE)0, (VALUE)1 }
  };
  return
      Mat4x4T<VALUE>(InitRot,
        Vec3T<VALUE>(axes[axis1][0], axes[axis1][1], axes[axis1][2]),
        rot1)
    * Mat4x4T<VALUE>(InitRot,
        Vec3T<VALUE>(axes[axis2][0], axes[axis2][1], axes[axis2][2]),
        rot2)
    * Mat4x4T<VALUE>(InitRot,
        Vec3T<VALUE>(axes[axis3][0], axes[axis3][1], axes[axis3][2]),
        rot3);
}

/* decomposes a rotation matrix into EULER angles.
 *
 * It is necessary that the upper left 3x3 matrix is composed of rotations
 * only. Translational parts are not considered.
 * Other transformations (e.g. scaling, shearing, projection) may cause
 * wrong results.
 */
template <typename VALUE>
void decompose(
  const Mat4x4T<VALUE> &mat,
  RotAxis axis1, RotAxis axis2, RotAxis axis3,
  VALUE &angle1, VALUE &angle2, VALUE &angle3)
{
  assert(axis1 != axis2 && axis2 != axis3);
  /* This is ported from EulerAngles.h of the Eigen library. */
  const int odd = (axis1 + 1) % 3 == axis2 ? 0 : 1;
  const int i = axis1;
  const int j = (axis1 + 1 + odd) % 3;
  const int k = (axis1 + 2 - odd) % 3;
  if (axis1 == axis3) {
    angle1 = std::atan2(mat[j][i], mat[k][i]);
    if ((odd && angle1 < (VALUE)0) || (!odd && angle1 > (VALUE)0)) {
      angle1 = angle1 > (VALUE)0 ? angle1 - (VALUE)Pi : angle1 + (VALUE)Pi;
      const VALUE s2 = length(Vec2T<VALUE>(mat[j][i], mat[k][i]));
      angle2 = -std::atan2(s2, mat[i][i]);
    } else {
      const VALUE s2 = length(Vec2T<VALUE>(mat[j][i], mat[k][i]));
      angle2 = std::atan2(s2, mat[i][i]);
    }
    const VALUE s1 = sin(angle1);
    const VALUE c1 = cos(angle1);
    angle3 = std::atan2(c1 * mat[j][k] - s1 * mat[k][k],
      c1 * mat[j][j] - s1 * mat[k][j]);
  } else {
    angle1 = atan2(mat[j][k], mat[k][k]);
    const VALUE c2 = length(Vec2T<VALUE>(mat[i][i], mat[i][j]));
    if ((odd && angle1<(VALUE)0) || (!odd && angle1 > (VALUE)0)) {
      angle1 = (angle1 > (VALUE)0)
        ? angle1 - (VALUE)Pi : angle1 + (VALUE)Pi;
      angle2 = std::atan2(-mat[i][k], -c2);
    } else angle2 = std::atan2(-mat[i][k], c2);
    const VALUE s1 = std::sin(angle1);
    const VALUE c1 = std::cos(angle1);
    angle3 = std::atan2(s1 * mat[k][i] - c1 * mat[j][i],
      c1 * mat[j][j] - s1 * mat[k][j]);
  }
  if (!odd) {
    angle1 = -angle1; angle2 = -angle2; angle3 = -angle3;
  }
}

template <typename VALUE>
void decompose(
  const Mat4x4T<VALUE> &mat,
  EulerAngle modeEuler, VALUE &angle1, VALUE &angle2, VALUE &angle3)
{
  RotAxis axis1, axis2, axis3;
  decompose(modeEuler, axis1, axis2, axis3);
  decompose(mat, axis1, axis2, axis3, angle1, angle2, angle3);
}

#endif // LIN_MATH_H