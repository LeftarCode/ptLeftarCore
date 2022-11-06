#include "Sphere.h"

constexpr float tMin = 0.0f;
constexpr float tMax = std::numeric_limits<float>::max();
constexpr float tEpsilon = std::numeric_limits<float>::epsilon();

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
  Vector3f oc = ray.origin - center;
  float a = ray.direction.dotProduct(ray.direction);
  float b = oc.dotProduct(ray.direction);
  float c = oc.dotProduct(oc) - radius * radius;

  float discriminant = b * b - a * c;
  if (discriminant > 0) {
    float temp = -(b + sqrt(b * b - a * c)) / a;
    if (temp < tMax && temp > tMin) {
      Vector3f tempPos = ray.origin + ray.direction * temp;
      hitDescriptor.normal = tempPos - center;
      hitDescriptor.normal.normalize();
      hitDescriptor.position = center + hitDescriptor.normal * (radius * 1.01f);
      hitDescriptor.materialId = materialId;
      hitDescriptor.uv = Vector3f(0, 0, 0);
      hitDescriptor.primitive = this;
      return true;
    }
    temp = (-b + sqrtf(b * b - a * c)) / a;
    if (temp < tMax && temp > tMin) {
      Vector3f tempPos = ray.origin + ray.direction * temp;
      hitDescriptor.normal = tempPos - center;
      hitDescriptor.normal.normalize();
      hitDescriptor.position = center + hitDescriptor.normal * (radius * 1.01f);
      hitDescriptor.materialId = materialId;
      hitDescriptor.uv = Vector3f(0, 0, 0);
      hitDescriptor.primitive = this;
      return true;
    }
  }
  return false;
}

BoundingBox Sphere::getBoundingBox() const {
  return boundingBox;
}
