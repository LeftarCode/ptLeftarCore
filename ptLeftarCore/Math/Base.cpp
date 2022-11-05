#include "Base.h"

#include <random>

void __vectorcall multiCross(__m256 result[3],
                             const __m256 a[3],
                             const __m256 b[3]) {
  result[0] = _mm256_fmsub_ps(a[1], b[2], _mm256_mul_ps(b[1], a[2]));
  result[1] = _mm256_fmsub_ps(a[2], b[0], _mm256_mul_ps(b[2], a[0]));
  result[2] = _mm256_fmsub_ps(a[0], b[1], _mm256_mul_ps(b[0], a[1]));
}

__m256 __vectorcall multiDot(const __m256 a[3], const __m256 b[3]) {
  return _mm256_fmadd_ps(
      a[2], b[2], _mm256_fmadd_ps(a[1], b[1], _mm256_mul_ps(a[0], b[0])));
}

void __vectorcall multiSub(__m256 result[3],
                           const __m256 a[3],
                           const __m256 b[3]) {
  result[0] = _mm256_sub_ps(a[0], b[0]);
  result[1] = _mm256_sub_ps(a[1], b[1]);
  result[2] = _mm256_sub_ps(a[2], b[2]);
}

float randomFloat(float min, float max) {
  static std::uniform_real_distribution<float> distribution(min, max);
  static std::mt19937 generator;
  return distribution(generator);
}

float clamp(float value, float min, float max) {
  return value < min ? min : (value > max ? max : value);
}
