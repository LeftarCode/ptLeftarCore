#pragma once
#include "../../Math/Vector3.h"
#include "../Color.h"

class Technique {
 public:
  virtual Color scatter(Primitive::HitDescriptor hit,
                        const Vector3f& incoming,
                        Vector3f& outcoming) = 0;
};
