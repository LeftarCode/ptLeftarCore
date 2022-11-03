#pragma once
#include <stdint.h>
#include <math.h>

struct Color {
  uint8_t r, g, b;
};

static inline Color blendColor(Color a, Color b, float blendingFactor) {
  Color outColor{0, 0, 0};
  outColor.r =
      sqrt((1.0f - blendingFactor) * a.r * a.r + blendingFactor * b.r * b.r);
  outColor.g =
      sqrt((1.0f - blendingFactor) * a.g * a.g + blendingFactor * b.g * b.g);
  outColor.b =
      sqrt((1.0f - blendingFactor) * a.b * a.b + blendingFactor * b.b * b.b);

  return outColor;
}