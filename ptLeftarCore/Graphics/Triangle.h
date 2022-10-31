#pragma once
#include "Primitive.h"
#include "Color.h"
#include "../Math/Ray.h"
#include "Vertex.h"
#include "AABB.h"

class Triangle : public Primitive {
public:
  Vertex v1, v2, v3;
  float denom;

public:
  Vector3f normal;
  Triangle(Vertex v1, Vertex v2, Vertex v3);
  virtual bool hit(Ray ray, Vector3f& barycentricCords);
  AABB getBoundingBox();
};