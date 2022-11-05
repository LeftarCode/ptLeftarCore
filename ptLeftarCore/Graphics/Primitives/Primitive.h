#pragma once
#include "../../Math/BoundingBox.h"
#include "../../Math/Ray.h"
#include "../Color.h"

enum PrimitiveType { eTriangle, eSphere };

class Primitive {
 public:
  struct HitDescriptor {
    Vector3f position, normal, uv;
    Primitive* primitive = nullptr;
    int materialId;
  };

  Primitive(PrimitiveType type);
  virtual bool hit(const Ray& ray, Primitive::HitDescriptor& hitDescriptor) = 0;
  virtual BoundingBox getBoundingBox() const = 0;
  PrimitiveType getType() const { return type; };

 protected:
  PrimitiveType type;
};
