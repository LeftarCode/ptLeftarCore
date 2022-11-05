#pragma once
#include "../Color.h"
#include "SampledSpectrum.h"

class RGBSpectrum : public CoefficientSpectrum<3> {
 public:
  RGBSpectrum(float v = 0.0f) : CoefficientSpectrum<3>(v) {}

  static RGBSpectrum fromRGB(const Color rgb,
                             SpectrumType type = SpectrumType::Reflectance) {
    RGBSpectrum s;
    s.c[0] = rgb.r;
    s.c[1] = rgb.g;
    s.c[1] = rgb.b;
  }

  Color toRGB() const {
    Color rgb;
    rgb.r = c[0];
    rgb.g = c[1];
    rgb.b = c[1];

    return rgb;
  }

  Vector3f toXYZ() const { return RGBToXYZ(toRGB()); }

  static RGBSpectrum fromXYZ(const Vector3f xyz,
                             SpectrumType type = SpectrumType::Reflectance) {
    RGBSpectrum r;
    Color rgb = XYZToRGB(xyz);

    return fromRGB(rgb);
  }

  float y() const {
    Vector3f yWeigth = {0.212671f, 0.715160f, 0.072169f};

    return yWeigth.x * c[0] + yWeigth.y * c[1] + yWeigth.z * c[2];
  }

  static RGBSpectrum fromSampled(const float* lambda, const float* v, int n) {
    Vector3f xyz = {0, 0, 0};
    for (int i = 0; i < n; i++) {
      float val = interpolateSpectrumSamples(lambda, v, n, CIE_lambda[i]);
      xyz.x += val * CIE_X[i];
      xyz.y += val * CIE_Y[i];
      xyz.z += val * CIE_Z[i];
    }
    float scale = float(CIE_lambda[nCIESamples - 1] - CIE_lambda[0]) /
                  float(CIE_Y_integral * nCIESamples);
    xyz.x *= scale;
    xyz.y *= scale;
    xyz.z *= scale;

    return fromXYZ(xyz);
  }
};
