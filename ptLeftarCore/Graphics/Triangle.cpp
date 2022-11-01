#include "Triangle.h"
#include <algorithm>


static float AreaOfTriangle(float a, float b, float c) {
  float p = (a + b + c) / 2.0;
  float area_sqr = p * (p - a) * (p - b) * (p - c);

  // It seems that due to floating point inaccuracies it's possible to get a
  // negative result here when we are dealing with a triangle having all points
  // on the same line (i.e. with a zero size area).
  if (area_sqr < 0.0) {
    return 0.0;
  }

  return sqrt(area_sqr);
}

Triangle::Triangle(Vertex v1, Vertex v2, Vertex v3)
    : Primitive(PrimitiveType::eTriangle), v1(v1), v2(v2), v3(v3) {
  Vector3f v0v1 = v2.position - v1.position;
  Vector3f v0v2 = v3.position - v1.position;
  normal = v0v1.crossProduct(v0v2);
  normal.normalize();
  denom = normal.dotProduct(normal);
}

bool Triangle::hit(const Ray &ray, Primitive::HitDescriptor &hitDescriptor) {
  AABB aabb = getBoundingBox();
  Vector3f dirfrac = ray.invDirection;

  float t1 = (aabb.min.x - ray.origin.x) * dirfrac.x;
  float t2 = (aabb.max.x - ray.origin.x) * dirfrac.x;
  float t3 = (aabb.min.y - ray.origin.y) * dirfrac.y;
  float t4 = (aabb.max.y - ray.origin.y) * dirfrac.y;
  float t5 = (aabb.min.z - ray.origin.z) * dirfrac.z;
  float t6 = (aabb.max.z - ray.origin.z) * dirfrac.z;

  float tmax =
      std::min({std::max(t1, t2), std::max(t3, t4), std::max(t5, t6)});
  if (tmax < 0.0) {
    return false;
  }

  float tmin =
      std::max({std::min(t1, t2), std::min(t3, t4), std::min(t5, t6)});
  if (tmin > tmax) {
    return false;
  }

  Vector3f e1 = v2.position - v1.position;
  Vector3f e2 = v3.position - v1.position;
  Vector3f pvec = ray.direction.crossProduct(e2);
  float det = e1.dotProduct(pvec);

  if (fabs(det) < 0.000000001) {
    return false;
  }

  float inv_det = 1.0 / det;
  Vector3f tvec = ray.origin - v1.position;
  float u = tvec.dotProduct(pvec) * inv_det;
  if (u < 0.0 || u > 1.0) {
    return false;
  }

  Vector3f qvec = tvec.crossProduct(e1);
  float v = ray.direction.dotProduct(qvec) * inv_det;
  if (v < 0.0 || u + v > 1.0) {
    return false;
  }

  float final_distance = e2.dotProduct(qvec) * inv_det;
  if (final_distance < 0.0) {
    return false;
  }

  Vector3f point = ray.origin + ray.direction * final_distance; 

  float a = v1.position.distance(v2.position);
  float b = v2.position.distance(v3.position);
  float c = v3.position.distance(v1.position);

  float p0 = point.distance(v1.position);
  float p1 = point.distance(v2.position);
  float p2 = point.distance(v3.position);

  float n0 = AreaOfTriangle(b, p2, p1);
  float n1 = AreaOfTriangle(c, p0, p2);
  float n2 = AreaOfTriangle(a, p1, p0);

  float n = n0 + n1 + n2;

  hitDescriptor.position = point;
  hitDescriptor.normal = (v1.normal * n0 + v2.normal * n1 + v3.normal * n2) / n;
  hitDescriptor.uv = (v1.uv * n0 + v2.uv * n1 + v3.uv * n2) / n;
  hitDescriptor.primitive = this;

  return true;
}

AABB Triangle::getBoundingBox() const { 
  AABB aabb(v1.position, v1.position);
  aabb.extend(v2.position);
  aabb.extend(v3.position);

  return aabb;
}