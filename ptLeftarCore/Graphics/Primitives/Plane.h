#pragma once
#include "Primitive.h"

class Plane : public Primitive {
 public:
  Vector3f center;
  Vector3f normal;
  int materialId;

  Plane(Vector3f center, Vector3f normal, int materialId)
      : Primitive(ePlane), center(center), materialId(materialId) {
    normal.normalize();
    this->normal = normal;
  }
  virtual bool hit(const Ray& ray, Primitive::HitDescriptor& hitDescriptor) {
    Vector3f negativeNormal = normal;
    negativeNormal.x = -negativeNormal.x;
    negativeNormal.y = -negativeNormal.y;
    negativeNormal.z = -negativeNormal.z;

    float denom = negativeNormal.dotProduct(ray.direction);
    float t = 0.0f;
    if (denom > 1e-6) {
      Vector3f co = center - ray.origin;
      t = co.dotProduct(negativeNormal) / denom;
    }

    if (t > 0) {
      hitDescriptor.uv = Vector3f();
      hitDescriptor.position = ray.origin + ray.direction * t;
      hitDescriptor.position.y = center.y;
      hitDescriptor.normal = normal;
      hitDescriptor.materialId = materialId;
      hitDescriptor.primitive = this;
      return true;
    }

    return false;
  }
  Primitive::HitDescriptor getHitDescriptorFromPoint(Vector3f point) {
    return HitDescriptor();
  }
  virtual BoundingBox getBoundingBox() const { return BoundingBox(); }
};
