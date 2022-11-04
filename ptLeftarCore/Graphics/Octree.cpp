#include "Octree.h"
#include <limits>

Octree::Octree(std::vector<Triangle> &triangles, std::vector<Sphere> &spheres) {
  BoundingBox aabb = BoundingBox();
  for (auto triangle : triangles) {
    aabb.extend(triangle.v1.position);
    aabb.extend(triangle.v2.position);
    aabb.extend(triangle.v3.position);
  }

  for (auto sphere : spheres) {
    aabb.extend(sphere.center + sphere.radius);
    aabb.extend(sphere.center - sphere.radius);
  }
  
  rootNode = new OctreeNode(aabb);
  for (auto triangle : triangles) {
    rootNode->addTriangle(triangle);
  }
  for (auto sphere : spheres) {
    rootNode->addSphere(sphere);
  }
  rootNode->subdivide();
}

bool Octree::hit(const Ray& ray, Primitive::HitDescriptor &hitDescriptor) const {
  return rootNode->hit(ray, hitDescriptor);
}