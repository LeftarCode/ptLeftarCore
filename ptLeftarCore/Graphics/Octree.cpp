#include "Octree.h"
#include <limits>

Octree::Octree(std::vector<Triangle>& scene) {
  AABB aabb = AABB();
  for (auto triangle : scene) {
    aabb.extend(triangle.v1.position);
    aabb.extend(triangle.v2.position);
    aabb.extend(triangle.v3.position);
  }
  
  rootNode = new OctreeNode(aabb.min, aabb.max);
  for (auto triangle : scene) {
    rootNode->addTriangle(triangle);
  }
  rootNode->subdivide();
}

bool Octree::hit(const Ray& ray, Primitive::HitDescriptor &hitDescriptor) const {
  return rootNode->hit(ray, hitDescriptor);
}