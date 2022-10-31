#pragma once
#include "../Math/Vector3.h"
#include "../Math/Ray.h"

class AABB {
public:
  AABB(Vector3f topLeftFront, Vector3f bottomRightBack);
  void extend(Vector3f point);
  bool contains(Vector3f point);
  bool fullyContains(AABB aabb);
  bool intersect(Ray ray);

  Vector3f topLeftFront, bottomRightBack;
};