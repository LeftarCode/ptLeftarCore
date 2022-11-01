#include "Triangle.h"
#include <algorithm>


Triangle::Triangle(Vertex v1, Vertex v2, Vertex v3)
    : Primitive(PrimitiveType::eTriangle), v1(v1), v2(v2), v3(v3) {
  Vector3f v0v1 = v2.position - v1.position;
  Vector3f v0v2 = v3.position - v1.position;
  normal = v0v1.crossProduct(v0v2);
  denom = normal.dotProduct(normal);
}

// TODO(LeftarCode): Implement fast RT intersection https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
bool Triangle::hit(const Ray &ray, Vector3f &outNormal) {
  if (fabs(normal.dotProduct(ray.direction) < 0.000001)) {
    return false;
  }

  float D = -normal.dotProduct(v1.position);
  float t =
      -(normal.dotProduct(ray.origin) + D) /
      normal.dotProduct(ray.direction);

  if (t < 0) {
    return false;
  }

  Vector3f hitPoint = ray.origin + ray.direction * t;

  Vector3f edge1 = v2.position - v1.position;
  Vector3f vp0 = hitPoint - v1.position;
  Vector3f C = edge1.crossProduct(vp0);
  if (normal.dotProduct(C) < 0) {
    return false;
  }

  Vector3f edge2 = v3.position - v2.position;
  Vector3f vp1 = hitPoint - v2.position;
  C = edge2.crossProduct(vp1);
  float u = normal.dotProduct(C) / denom;
  if (normal.dotProduct(C) < 0) {
    return false;
  }

  Vector3f edge3 = v1.position - v3.position;
  Vector3f vp2 = hitPoint - v3.position;
  C = edge3.crossProduct(vp2);
  float v = normal.dotProduct(C) / denom;
  if (normal.dotProduct(C) < 0) {
    return false;
  }

  float w = 1 - u - v;
  outNormal = Vector3f(v1.normal * u +
                        v2.normal * v +
                        v3.normal * w);

  return true;
}

AABB Triangle::getBoundingBox() { 
  AABB aabb(v1.position, v1.position);
  aabb.extend(v2.position);
  aabb.extend(v3.position);

  return aabb;
}