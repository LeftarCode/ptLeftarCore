#pragma once
#include "../Primitives/Primitive.h"
#include "Technique.h"

Vector3f reflect(const Vector3f& v, const Vector3f& n) {
  return v - n * 2 * (v.dotProduct(n));
}

class BRDFTechnique : public Technique {
 public:
  virtual Color scatter(Primitive::HitDescriptor hit,
                        const Vector3f& incoming,
                        Vector3f& outcoming) {
    outcoming = reflect(incoming, hit.normal);
    return Color{1, 1, 1};
  }
};
