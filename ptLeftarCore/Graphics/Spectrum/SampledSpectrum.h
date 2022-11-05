#pragma once
#include <algorithm>
#include <vector>
#include "../../Graphics/Color.h"
#include "../../Math/Vector3.h"
#include "CoefficientSpectrum.h"

static const int sampledLambdaStart = 400;
static const int sampledLambdaEnd = 700;
static const int sampledSamplesCount = 60;

static const int nCIESamples = 471;
extern const float CIE_X[nCIESamples];
extern const float CIE_Y[nCIESamples];
extern const float CIE_Z[nCIESamples];
extern const float CIE_lambda[nCIESamples];
static const float CIE_Y_integral = 106.856895;
static const int nRGB2SpectSamples = 32;
extern const float RGB2SpectLambda[nRGB2SpectSamples];
extern const float RGBRefl2SpectWhite[nRGB2SpectSamples];
extern const float RGBRefl2SpectCyan[nRGB2SpectSamples];
extern const float RGBRefl2SpectMagenta[nRGB2SpectSamples];
extern const float RGBRefl2SpectYellow[nRGB2SpectSamples];
extern const float RGBRefl2SpectRed[nRGB2SpectSamples];
extern const float RGBRefl2SpectGreen[nRGB2SpectSamples];
extern const float RGBRefl2SpectBlue[nRGB2SpectSamples];
extern const float RGBIllum2SpectWhite[nRGB2SpectSamples];
extern const float RGBIllum2SpectCyan[nRGB2SpectSamples];
extern const float RGBIllum2SpectMagenta[nRGB2SpectSamples];
extern const float RGBIllum2SpectYellow[nRGB2SpectSamples];
extern const float RGBIllum2SpectRed[nRGB2SpectSamples];
extern const float RGBIllum2SpectGreen[nRGB2SpectSamples];
extern const float RGBIllum2SpectBlue[nRGB2SpectSamples];

bool isSpectrumSamplesSorted(const float* lambda, const float* vals, int n);
inline Color XYZToRGB(const Vector3f& xyz);
inline Vector3f RGBToXYZ(const Color& rgb);
void sortSpectrumSamples(float* lambda, float* vals, int n);
float interpolateSpectrumSamples(const float* lambda,
                                 const float* vals,
                                 int n,
                                 float l);

class SampledSpectrum : public CoefficientSpectrum<sampledSamplesCount> {
 public:
  SampledSpectrum(float v = 0.0f);

  static SampledSpectrum fromSampled(const float* lambda,
                                     const float* v,
                                     int n);
  static void init();
  static float averageSpectrumSamples(const float* lambda,
                                      const float* vals,
                                      int n,
                                      float lambdaStart,
                                      float lambdaEnd);

  Vector3f toXYZ() const;
  Color toRGB() const;
  float y() const;

 private:
  static SampledSpectrum X, Y, Z;
  static SampledSpectrum rgbRefl2SpectWhite, rgbRefl2SpectCyan;
  static SampledSpectrum rgbRefl2SpectMagenta, rgbRefl2SpectYellow;
  static SampledSpectrum rgbRefl2SpectRed, rgbRefl2SpectGreen;
  static SampledSpectrum rgbRefl2SpectBlue;
  static SampledSpectrum rgbIllum2SpectWhite, rgbIllum2SpectCyan;
  static SampledSpectrum rgbIllum2SpectMagenta, rgbIllum2SpectYellow;
  static SampledSpectrum rgbIllum2SpectRed, rgbIllum2SpectGreen;
  static SampledSpectrum rgbIllum2SpectBlue;
};
