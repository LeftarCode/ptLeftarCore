#pragma once
#include "../Graphics/Triangle.h"
#include "../Graphics/Sphere.h"
#include <vector>

class OctreeNode {
  std::vector<OctreeNode*> children;
  std::vector<Triangle> triangles;
  std::vector<Sphere> spheres;
  std::vector<PackedTriangles> packedTriangles;
  Vector3f topLeftFront, bottomRightBack;

  void generatePackedTriangles();
  void generateChildren();
public:
  OctreeNode();
  OctreeNode(Vector3f topLeftFront, Vector3f bottomRightBack);
  void addTriangle(Triangle triangles);
  void addSphere(Sphere sphere);
  void subdivide();
  bool hit(Ray ray, Primitive::HitDescriptor& hitDescriptor) const;
};