#pragma once
#include "../Graphics/Triangle.h"
#include <vector>

class OctreeNode {
  std::vector<OctreeNode*> children;
  std::vector<Triangle> triangles;
  Vector3f topLeftFront, bottomRightBack;

public:
  OctreeNode();
  OctreeNode(Vector3f topLeftFront, Vector3f bottomRightBack);
  void addTriangle(Triangle triangles);
  void subdivide();
  bool hit(Ray ray, Primitive::HitDescriptor& hitDescriptor) const;
};