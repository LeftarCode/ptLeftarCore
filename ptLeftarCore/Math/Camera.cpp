#include "Camera.h"

float Convert(float degree) {
  float pi = 3.141592f;
  return (degree * (pi / 180));
}

Camera::Camera(Vector3f origin,
               float viewportWidth,
               float viewportHeight,
               float fieldOfView)
    : origin(origin),
      viewportWidth(viewportWidth),
      viewportHeight(viewportHeight),
      fieldOfView(fieldOfView) {
  float aspect = viewportHeight / viewportWidth;
  horizontal = Vector3f{1.0f, 0.0f, 0.0f};
  vertical = Vector3f{0.0f, aspect, 0.0f};
  lowerLeftCorenr = origin - horizontal / 2 - vertical / 2 -
                    Vector3f(0.0f, 0.0f, focalLength);

  lookAt(origin + Vector3f(0, 0, -1.0f));
}

Ray Camera::castRay(int viewportX, int viewportY) {
  Vector3f viewPlanePoint = viewPlaneBottomLeftPoint + xIncVector * viewportX +
                            yIncVector * viewportY;

  Ray r;
  r.origin = origin;
  r.direction = viewPlanePoint - origin;
  r.direction.normalize();
  r.invDirection = Vector3f(1.0f / r.direction.x, 1.0f / r.direction.y,
                            1.0f / r.direction.z);
  r.invDirection.normalize();
  return r;
}

void Camera::lookAt(const Vector3f& point) {
  Vector3f lookAtPoint = point;

  Vector3f viewDirection = lookAtPoint - origin;
  viewDirection.normalize();

  Vector3f u = viewDirection.crossProduct(Vector3f(0, -1, 0));
  Vector3f v = u.crossProduct(viewDirection);

  float viewPlaneHalfWidth = tanf(Convert(fieldOfView));
  float aspectRation = viewportHeight / viewportWidth;
  float viewPlaneHalfHeight = aspectRation * viewPlaneHalfWidth;

  viewPlaneBottomLeftPoint =
      lookAtPoint - (v * viewPlaneHalfHeight) - (u * viewPlaneHalfWidth);
  xIncVector = (u * 2 * viewPlaneHalfWidth) / viewportWidth;
  yIncVector = (v * 2 * viewPlaneHalfHeight) / viewportHeight;
}
