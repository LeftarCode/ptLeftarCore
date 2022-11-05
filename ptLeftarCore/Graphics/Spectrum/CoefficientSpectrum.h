#pragma once
#include <math.h>
#include <limits>
#include "../../Math/Base.h"
#include "../../Math/Vector3.h"
#include "../Color.h"

enum SpectrumType { Reflectance, Illuminant };

template <int sampleCount>
class CoefficientSpectrum {
 public:
  CoefficientSpectrum(float v = 0.0f) {
    for (int i = 0; i < sampleCount; i++) {
      c[i] = v;
    }
  }

  CoefficientSpectrum& operator+=(const CoefficientSpectrum& s) {
    for (int i = 0; i < sampleCount; i++) {
      c[i] += s.c[i];
    }

    return *this;
  }

  CoefficientSpectrum operator+(const CoefficientSpectrum& s) {
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < sampleCount; i++) {
      ret.c[i] += s.c[i];
    }

    return ret;
  }

  CoefficientSpectrum operator-(const CoefficientSpectrum& s) {
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < sampleCount; i++) {
      ret.c[i] -= s.c[i];
    }

    return ret;
  }

  CoefficientSpectrum operator/(const CoefficientSpectrum& s) {
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < sampleCount; i++) {
      ret.c[i] -= s.c[i];
    }

    return ret;
  }

  CoefficientSpectrum operator*(const CoefficientSpectrum& s) {
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < sampleCount; i++) {
      ret.c[i] -= s.c[i];
    }

    return ret;
  }

  CoefficientSpectrum& operator*=(float v) {
    for (int i = 0; i < sampleCount; i++) {
      c[i] *= v;
    }

    return *this;
  }

  friend inline CoefficientSpectrum operator*(float v,
                                              const CoefficientSpectrum& s) {
    return s * v;
  }

  CoefficientSpectrum operator/(float v) {
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < sampleCount; i++) {
      ret.c[i] /= v;
    }
    static_assert(!ret.hasNaNs());
    return ret;
  }

  CoefficientSpectrum& operator/=(float v) {
    for (int i = 0; i < sampleCount; i++) {
      c[i] /= v;
    }

    return *this;
  }

  bool operator==(const CoefficientSpectrum& s) const {
    for (int i = 0; i < sampleCount; i++) {
      if (c[i] != s.c[i]) {
        return false;
      }
    }

    return true;
  }

  bool operator!=(const CoefficientSpectrum& s) const { return !(*this == s); }

  bool isBlack() const {
    for (int i = 0; i < sampleCount; i++) {
      if (c[i] != 0.0f) {
        return false;
      }
    }

    return true;
  }

  friend CoefficientSpectrum sqrt(const CoefficientSpectrum& s) {
    CoefficientSpectrum ret;
    for (int i = 0; i < sampleCount; ++i) {
      ret.c[i] = sqrtf(s.c[i]);
    }
    return ret;
  }
  template <int n>
  friend inline CoefficientSpectrum<n> pow(const CoefficientSpectrum<n>& s,
                                           float e);
  CoefficientSpectrum operator-() const {
    CoefficientSpectrum ret;
    for (int i = 0; i < sampleCount; ++i) {
      ret.c[i] = -c[i];
    }
    return ret;
  }

  friend CoefficientSpectrum exponent(const CoefficientSpectrum& s) {
    CoefficientSpectrum ret;
    for (int i = 0; i < sampleCount; ++i) {
      ret.c[i] = expf(s.c[i]);
    }
    return ret;
  }

  CoefficientSpectrum clamp(
      float low = 0,
      float high = std::numeric_limits<float>::max()) const {
    CoefficientSpectrum ret;
    for (int i = 0; i < sampleCount; ++i) {
      ret.c[i] = ::clamp(c[i], low, high);
    }
    return ret;
  }

  float& operator[](int i) { return c[i]; }
  float operator[](int i) const { return c[i]; }

 protected:
  float c[sampleCount];
};
