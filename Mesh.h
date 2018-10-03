#ifndef MESH_H
#define MESH_H

#include <vector>

#include "linmath.h"
#include "util.h"

struct VertexC {
  typedef Vec3f Coord;
  typedef void Normal;
  typedef void Color;
  typedef void TexCoord;

  Coord coord;
};

struct VertexCN {
  typedef Vec3f Coord;
  typedef Vec3f Normal;
  typedef void Color;
  typedef void TexCoord;

  Coord coord;
  Normal normal;
};

struct VertexCNT {
  typedef Vec3f Coord;
  typedef Vec3f Normal;
  typedef void Color;
  typedef Vec2f TexCoord;

  Coord coord;
  Normal normal;
  TexCoord texCoord;

  VertexCNT() { }
  VertexCNT(
    const Coord &coord, const Normal &normal, const TexCoord &texCoord):
    coord(coord), normal(normal), texCoord(texCoord)
  { }
};

struct VertexCNCT {
  typedef Vec3f Coord;
  typedef Vec3f Normal;
  typedef Vec4f Color;
  typedef Vec2f TexCoord;

  Coord coord;
  Normal normal;
  Color color;
  TexCoord texCoord;
};

template <typename VERTEX>
struct storeCoord {
  storeCoord(VERTEX &vtx, const Vec3f &coord)
  {
    vtx.coord = coord;
  }
};

template <typename VERTEX, typename NORMAL = typename VERTEX::Normal>
struct storeNormal {
  storeNormal(VERTEX &vtx, const Vec3f &normal)
  {
    vtx.normal = normal;
  }
};
template <typename VERTEX>
struct storeNormal<VERTEX, void> {
  storeNormal(VERTEX&, const Vec3f&) { }
};

template <typename VERTEX, typename COLOR = typename VERTEX::Color>
struct storeColor {
  storeColor(VERTEX &vtx, const Vec4f &color)
  {
    vtx.color = color;
  }
};
template <typename VERTEX>
struct storeColor<VERTEX, void> {
  storeColor(VERTEX&, const Vec4f&) { }
};

template <typename VERTEX, typename TEXCOORD = typename VERTEX::TexCoord>
struct storeTexCoord {
  storeTexCoord(VERTEX &vtx, const Vec2f &texCoord)
  {
    vtx.texCoord = texCoord;
  }
};
template <typename VERTEX>
struct storeTexCoord<VERTEX, void> {
  storeTexCoord(VERTEX&, const Vec2f&) { }
};

/* stores a mesh of triangles.
 *
 * The mesh may be indexed or non-indexed.
 * (In the latter case, the idcs are left empty which can be used as
 * indicator also.)
 *
 * Indexed meshes may share vertices reducing the total memory of storage.
 */
template <typename VERTEX, typename INDEX = uint>
struct MeshT {
  typedef VERTEX Vertex;
  typedef INDEX Index;
  // vertices
  std::vector<Vertex> vtcs;
  std::vector<uint> idcs;
};

template <typename VERTEX>
struct MeshT<VERTEX, void> {
  typedef VERTEX Vertex;
  typedef void Index;
  // vertices
  std::vector<Vertex> vtcs;
};

#endif // MESH_H
