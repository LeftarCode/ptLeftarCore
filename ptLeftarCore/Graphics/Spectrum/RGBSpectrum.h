#pragma once
#include "../Color.h"
#include "SampledSpectrum.h"

class RGBSpectrum : public CoefficientSpectrum<3> {
 public:
  RGBSpectrum(float v = 0.0f) : CoefficientSpectrum<3>(v) {}

  static RGBSpectrum fromRGB(const Color rgb,
                             SpectrumType type = SpectrumType::Reflectance);
  Color toRGB() const;
  Vector3f toXYZ() const;
  static RGBSpectrum fromXYZ(const Vector3f xyz,
                             SpectrumType type = SpectrumType::Reflectance);
  float y() const;
  static RGBSpectrum fromSampled(const float* lambda, const float* v, int n);
};
