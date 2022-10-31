#pragma once
#include "Ray.h"

class Camera {
  Vector3f origin;
  float viewportWidth, viewportHeight;
  float focalLength = 1.0f;

  Vector3f horizontal;
  Vector3f vertical;
  Vector3f lowerLeftCorenr;

public:
  Camera(Vector3f origin, float viewportWidth, float viewportHeight);
  Ray castRay(int viewportX, int viewportY);
};