#pragma once
#include "Texture.h"

enum MaterialType { eDiffusive, eReflective, eTransitive };

struct Material {
  Texture* diffuseTexture;
  MaterialType type;
  Color diffuseColor = Color{0, 0, 0};
  float roughness = 1.0f;
  float ior = 1.0f;
};
