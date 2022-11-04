#pragma once
#include "../Graphics/Triangle.h"
#include "../Graphics/Sphere.h"
#include <vector>

class OctreeNode {
  std::vector<OctreeNode*> children;
  std::vector<Triangle> triangles;
  std::vector<Sphere> spheres;
  std::vector<PackedTriangles> packedTriangles;
  BoundingBox aabb;

  void generatePackedTriangles();
  void generateChildren();
public:
  OctreeNode();
  OctreeNode(BoundingBox aabb);
  void addTriangle(Triangle triangles);
  void addSphere(Sphere sphere);
  void subdivide();
  bool hit(Ray ray, Primitive::HitDescriptor& hitDescriptor) const;
};