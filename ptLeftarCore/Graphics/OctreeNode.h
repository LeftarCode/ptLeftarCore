#pragma once
#include "../Graphics/Triangle.h"
#include <vector>

class OctreeNode {
  std::vector<OctreeNode*> children;
  std::vector<Triangle> triangles;
  Vector3f topLeftFront, bottomRightBack;

public:
  OctreeNode(Vector3f topLeftFront, Vector3f bottomRightBack);
  void addTriangle(Triangle triangles);
  void subdivide(int depth);
  bool hit(Ray ray, Vector3f &barycentricCords, Triangle& triangleHit);
};