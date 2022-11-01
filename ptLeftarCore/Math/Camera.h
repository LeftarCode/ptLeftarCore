#pragma once
#include "Ray.h"

class Camera {
public:
  Vector3f origin;
  float viewportWidth, viewportHeight;
  float focalLength = 1.0f;
  float fieldOfView = 0.0f;

  Vector3f horizontal;
  Vector3f vertical;
  Vector3f lowerLeftCorenr;

  Vector3f viewPlaneBottomLeftPoint, xIncVector, yIncVector;

public:
  Camera(Vector3f origin, float viewportWidth, float viewportHeight,
         float fieldOfView);
  Ray castRay(int viewportX, int viewportY);
  void lookAt(const Vector3f& point);
};