#pragma once
#include "Texture.h"

struct Material {
  Texture *diffuseTexture;

  Color diffuseColor = Color{0, 0, 0};
};