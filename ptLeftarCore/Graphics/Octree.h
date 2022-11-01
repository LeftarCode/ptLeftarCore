#pragma once
#include <vector>
#include "OctreeNode.h"

class Octree {
  OctreeNode *rootNode;

public:
  Octree(std::vector<Triangle> scene);
  bool hit(const Ray &ray, Primitive::HitDescriptor &hitDescriptor) const;
};