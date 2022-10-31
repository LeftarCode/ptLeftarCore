#pragma once
#include "Color.h"
#include "../Math/Ray.h"

enum PrimitiveType {
  eTriangle
};

class Primitive {
  PrimitiveType type;

public:
  Primitive(PrimitiveType type);
  virtual bool hit(Ray ray, Vector3f& barycentricCords) = 0;
};