#pragma once
#include <vector>

#include "../../Graphics/Primitives/Plane.h"
#include "../../Graphics/Primitives/Sphere.h"
#include "../../Graphics/Primitives/Triangle.h"

class OctreeNode {
  std::vector<OctreeNode*> children;
  std::vector<Triangle> triangles;
  std::vector<Sphere> spheres;
  std::vector<Plane> planes;
  std::vector<PackedTriangles> packedTriangles;
  BoundingBox aabb;

  void generatePackedTriangles();
  void generateChildren();

 public:
  OctreeNode();
  OctreeNode(BoundingBox aabb);
  void addTriangle(Triangle triangles);
  void addSphere(Sphere sphere);
  void addPlane(Plane plane);
  void subdivide();
  bool hit(Ray ray, Primitive::HitDescriptor& hitDescriptor) const;
};
