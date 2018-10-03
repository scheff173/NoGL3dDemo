#ifndef UTIL_H
#define UTIL_H

#include <cassert>
#include <cstdint>

typedef unsigned uint;
typedef std::uint32_t uint32;

template <typename VALUE>
VALUE clamp(VALUE value, VALUE min, VALUE max)
{
  return value < min ? min : value > max ? max : value;
}

inline int rnd(float value) { return (int)(value + 0.5f); }

inline bool isPowerOf2(uint value)
{
  return (value & (value - 1)) == 0;
}

template <typename VALUE, typename WEIGHT>
VALUE lerp(const VALUE &x0, const VALUE &x1, WEIGHT f0, WEIGHT f1)
{
  return f0 * x0 + f1 * x1;
}

template <typename VALUE, typename WEIGHT>
VALUE lerp(const VALUE &x0, const VALUE &x1, WEIGHT f)
{
  return ((WEIGHT)1 - f) * x0 + f * x1;
}

class Lock {
  private:
    bool &_lock;
  public:
    Lock(bool &lock): _lock(lock)
    {
      assert(!lock);
      _lock = true;
    }
    ~Lock() { _lock = false; }
    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;
};

#endif // UTIL_H
