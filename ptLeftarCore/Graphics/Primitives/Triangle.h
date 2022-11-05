#pragma once
#include <array>

#include "../../Math/Base.h"
#include "../../Math/BoundingBox.h"
#include "../../Math/Ray.h"
#include "../Color.h"
#include "../Vertex.h"
#include "Primitive.h"

class Triangle : public Primitive {
 public:
  Vertex v1, v2, v3;
  int materialId;

 public:
  Triangle(Vertex v1, Vertex v2, Vertex v3);
  Triangle();
  virtual bool hit(const Ray& ray, Primitive::HitDescriptor& hitDescriptor);
  Primitive::HitDescriptor getHitDescriptorFromPoint(Vector3f point);
  virtual BoundingBox getBoundingBox() const;
};

class PackedTriangles {
 public:
  __m256 e1[3];
  __m256 e2[3];
  __m256 v1[3];

  std::array<Triangle*, 8> triangles;
  bool __vectorcall hit(const PackedRay& packedRays,
                        PackedIntersectionResult& result,
                        Primitive::HitDescriptor& hitDescriptor);
};
