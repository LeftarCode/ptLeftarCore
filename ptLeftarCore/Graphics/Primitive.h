#pragma once
#include "Color.h"
#include "../Math/Ray.h"
#include "AABB.h"

enum PrimitiveType {
  eTriangle, eSphere
};

class Primitive {
  PrimitiveType type;


public:
  struct HitDescriptor {
    Vector3f position, normal, uv;
    Primitive *primitive = nullptr;
    int materialId;
  };

  Primitive(PrimitiveType type);
  virtual bool hit(const Ray &ray, Primitive::HitDescriptor &hitDescriptor) = 0;
  virtual AABB getBoundingBox() const = 0;
  PrimitiveType getType() const { return type; };
};