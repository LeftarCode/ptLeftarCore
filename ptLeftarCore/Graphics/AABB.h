#pragma once
#include "../Math/Vector3.h"
#include "../Math/Ray.h"

class AABB {
public:
  AABB(Vector3f topLeftFront, Vector3f bottomRightBack);
  AABB();
  void extend(Vector3f point);
  bool contains(Vector3f point);
  bool fullyContains(AABB aabb);
  bool intersect(Ray ray, float& distance);

  Vector3f min, max;
  Vector3f center;
};