#pragma once
#include "Primitive.h"

class Sphere : public Primitive {
public:
  Vector3f center;
  float radius;
  BoundingBox boundingBox;
  int materialId;

  Sphere(Vector3f center, float radius, int materialId);
  virtual bool hit(const Ray &ray, Primitive::HitDescriptor &hitDescriptor);
  Primitive::HitDescriptor getHitDescriptorFromPoint(Vector3f point);
  virtual BoundingBox getBoundingBox() const;
};