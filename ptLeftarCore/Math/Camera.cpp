#include "Camera.h"

Camera::Camera(Vector3f origin, float viewportWidth, float viewportHeight)
    : origin(origin), viewportWidth(viewportWidth),
      viewportHeight(viewportHeight) {
  float aspect = viewportHeight / viewportWidth;
  horizontal = Vector3f{1.0f, 0.0f, 0.0f};
  vertical = Vector3f{0.0f, aspect, 0.0f};
  lowerLeftCorenr = origin - horizontal / 2 - vertical / 2 -
                    Vector3f(0.0f, 0.0f, focalLength);
}

Ray Camera::castRay(int viewportX, int viewportY) {
  float u = viewportX / (float)viewportWidth;
  float v = viewportY / (float)viewportHeight;

  Ray r;
  r.origin = origin;
  r.direction = lowerLeftCorenr + horizontal * u + vertical * v - origin;
  r.invDirection = Vector3f(1 / r.direction.x, 1 / r.direction.y, 1 / r.direction.z);
  return r;
}