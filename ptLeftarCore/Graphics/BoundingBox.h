#pragma once
#include "../Math/Vector3.h"
#include "../Math/Ray.h"

class BoundingBox {
public:
  BoundingBox(Vector3f topLeftFront, Vector3f bottomRightBack);
  BoundingBox();
  void extend(Vector3f point);
  bool contains(Vector3f point);
  bool fullyContains(BoundingBox aabb);
  bool intersect(Ray ray, float& distance);

  Vector3f min, max;
  Vector3f center;
};