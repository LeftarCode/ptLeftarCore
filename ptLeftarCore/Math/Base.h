#pragma once
#include <immintrin.h>

static const __m256 IdentityM256 = _mm256_set1_ps(1.0f);
static const __m256 MinusIdentityM256 = _mm256_set1_ps(-1.0f);
static const __m256 PositiveEpsilonM256 = _mm256_set1_ps(1e-6f);
static const __m256 NegativeEpsilonM256 = _mm256_set1_ps(-1e-6f);
static const __m256 ZeroM256 = _mm256_set1_ps(0.0f);

void __vectorcall multiCross(__m256 result[3],
                             const __m256 a[3],
                             const __m256 b[3]);
__m256 __vectorcall multiDot(const __m256 a[3], const __m256 b[3]);
void __vectorcall multiSub(__m256 result[3],
                           const __m256 a[3],
                           const __m256 b[3]);
float randomFloat(float min, float max);
