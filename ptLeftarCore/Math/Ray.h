#pragma once
#include <immintrin.h>

#include <limits>

#include "./Vector3.h"

struct PackedRay {
  __m256 origin[3];
  __m256 direction[3];
};

struct PackedIntersectionResult {
  float t = std::numeric_limits<float>::max();
  int idx;
};

class Ray {
 public:
  Vector3f origin, direction, invDirection;
};
