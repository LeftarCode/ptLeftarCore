#pragma once
#include "Primitive.h"

class Sphere : public Primitive {
public:
  Vector3f center;
  float radius;
  AABB boundingBox;
  int materialId;

  Sphere(Vector3f center, float radius, int materialId);
  virtual bool hit(const Ray &ray, Primitive::HitDescriptor &hitDescriptor);
  Primitive::HitDescriptor getHitDescriptorFromPoint(Vector3f point);
  virtual AABB getBoundingBox() const;
};