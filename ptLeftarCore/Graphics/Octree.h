#pragma once
#include <vector>
#include "OctreeNode.h"

class Octree {
  OctreeNode *rootNode;
  Vector3f topLeftFront, bottomRightBack;
  int depth = 8;

public:
  Octree(std::vector<Triangle> scene);
  bool hit(Ray ray, Vector3f &barycentricCords, Triangle& triangle);
};