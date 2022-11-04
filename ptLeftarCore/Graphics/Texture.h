#pragma once
#include "Color.h"
#include "../Math/Vector3.h"
#include <string>

class Texture {
  ImageColor *data;
  int width, height, comp;

public:
  Color sample(Vector3f uv);
  void loadFromFile(std::string filename);
  ~Texture();
};