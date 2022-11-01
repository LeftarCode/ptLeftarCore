#pragma once
#include "Color.h"
#include "../Math/Ray.h"

enum PrimitiveType {
  eTriangle
};

class Primitive {
  PrimitiveType type;


public:
  struct HitDescriptor {
    Vector3f position, normal, uv;
    Primitive *primitive = nullptr;
  };

  Primitive(PrimitiveType type);
  virtual bool hit(const Ray &ray, Primitive::HitDescriptor &hitDescriptor) = 0;
};