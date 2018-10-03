#ifndef SPHERE_H
#define SPHERE_H

#include "mesh.h"

namespace {

template <typename MESH, bool FRONT>
void storeVertex(MESH &mesh, const Vec3f &coord)
{
  typedef typename MESH::Vertex Vertex;
  mesh.vtcs.push_back(Vertex());
  Vertex &vtx = mesh.vtcs.back();
  // coord
  storeCoord<Vertex>(vtx, coord);
  // normal
  typedef typename Vertex::Normal Normal;
  if (sizeof (Normal)) storeNormal<Vertex>(vtx, coord);
  // no colors
  // texture coordinate
  typedef typename Vertex::TexCoord TexCoord;
  if (sizeof (TexCoord)) {
    const Vec2f v = normalize(Vec2f(coord.x, coord.z), NoThrow);
    storeTexCoord<Vertex>(vtx,
      Vec2f(FRONT
        ? 1.0f - 0.5f * std::acos(clamp(v.x, -1.0f, 1.0f)) / (float)Pi
        : 0.5f * ::acos(clamp(v.x, -1.0f, 1.0f)) / (float)Pi,
        0.5f * coord.y + 0.5f));
  }
}

template <typename MESH, bool FRONT>
void makeSpherePatch(
  MESH &mesh, uint depth,
  const Vec3f &v1, const Vec3f &v2, const Vec3f &v3)
{
  if (depth) {
    const Vec3f v12 = normalize(v1 + v2, NoThrow);
    const Vec3f v23 = normalize(v2 + v3, NoThrow);
    const Vec3f v31 = normalize(v3 + v1, NoThrow);
    --depth;
    makeSpherePatch<MESH, FRONT>(mesh, depth, v1, v12, v31);
    makeSpherePatch<MESH, FRONT>(mesh, depth, v2, v23, v12);
    makeSpherePatch<MESH, FRONT>(mesh, depth, v3, v31, v23);
    makeSpherePatch<MESH, FRONT>(mesh, depth, v12, v23, v31);
  } else {
    storeVertex<MESH, FRONT>(mesh, v1);
    storeVertex<MESH, FRONT>(mesh, v2);
    storeVertex<MESH, FRONT>(mesh, v3);
  }
}

} // namespace

template <typename MESH>
void makeSphereMesh(MESH &mesh, uint depth)
{
  /* Octants:
   *       +----+     +----+
   *      /    /|    /    /|
   *     +----+ |   +----+ |
   *     |  2 | +   |  1 | +
   *   +----+ |/  +----+ |/
   *  /    /|-+  /    /|-+
   * +----+ |   +----+ |
   * |  3 | +---|  4 | +---+
   * |    |/   /|    |/   /|
   * +----+---+ +----+---+ |
   *     |  6 | +   |  5 | +
   *   +----+ |/  +----+ |/
   *  /    /|-+  /    /|-+
   * +----+ |   +----+ |
   * |  7 | +   |  8 | +
   * |    |/    |    |/
   * +----+     +----+
   * 
   *           y
   *        2  |     1
   *           | /
   *     3     |/ 4
   *     ------+------x
   *        6 /|     5
   *         / |
   *     7  z  |  8
   */
  // octant 1: top, left, back
  makeSpherePatch<MESH, false>(mesh, depth,
    Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, -1.0f),
    Vec3f(0.0f, 1.0f, 0.0f));
  // octant 2: top, right, back
  makeSpherePatch<MESH, false>(mesh, depth,
    Vec3f(0.0f, 0.0f, -1.0f), Vec3f(-1.0f, 0.0f, 0.0f),
    Vec3f(0.0f, 1.0f, 0.0f));
  // octant 3: top, right, front
  makeSpherePatch<MESH, true>(mesh, depth,
    Vec3f(-1.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f),
    Vec3f(0.0f, 1.0f, 0.0f));
  // octant 4: top, left, front
  makeSpherePatch<MESH, true>(mesh, depth,
    Vec3f(0.0f, 0.0f, 1.0f), Vec3f(1.0f, 0.0f, 0.0f),
    Vec3f(0.0f, 1.0f, 0.0f));
  // octant 5: bottom, left, back
  makeSpherePatch<MESH, false>(mesh, depth,
    Vec3f(0.0f, 0.0f, -1.0f), Vec3f(1.0f, 0.0f, 0.0f),
    Vec3f(0.0f, -1.0f, 0.0f));
  // octant 6: bottom, right, back
  makeSpherePatch<MESH, false>(mesh, depth,
    Vec3f(-1.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, -1.0f),
    Vec3f(0.0f, -1.0f, 0.0f));
  // octant 7: bottom, right, front
  makeSpherePatch<MESH, true>(mesh, depth,
    Vec3f(0.0f, 0.0f, 1.0f), Vec3f(-1.0f, 0.0f, 0.0f),
    Vec3f(0.0f, -1.0f, 0.0f));
  // octant 8: bottom, left, front
  makeSpherePatch<MESH, true>(mesh, depth,
    Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f),
    Vec3f(0.0f, -1.0f, 0.0f));
}

#endif // SPHERE_H
