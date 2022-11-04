#include "Sphere.h"

Sphere::Sphere(Vector3f center, float radius, int materialId)
    : Primitive(eSphere),
      center(center),
      radius(radius),
      materialId(materialId) {
  boundingBox = BoundingBox(center, center);
  boundingBox.extend(center + radius);
  boundingBox.extend(center - radius);
}

bool Sphere::hit(const Ray& ray, Primitive::HitDescriptor& hitDescriptor) {
  Vector3f m = ray.origin - center;
  float b = m.dotProduct(ray.direction);
  float c = m.dotProduct(m) - radius * radius;

  if (c > 0.0f && b > 0.0f) {
    return false;
  }

  float discr = b * b - c;

  if (discr < 0.0f) {
    return false;
  }

  float t = -b - sqrtf(discr);
  if (t < 0.0f) {
    t = 0.0f;
  }
  hitDescriptor.position = ray.origin + ray.direction * t;
  hitDescriptor.normal = center - hitDescriptor.position;
  hitDescriptor.normal.normalize();
  hitDescriptor.materialId = materialId;
  hitDescriptor.uv = Vector3f(0, 0, 0);
  hitDescriptor.primitive = this;

  return true;
}

BoundingBox Sphere::getBoundingBox() const {
  return boundingBox;
}
