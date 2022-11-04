#pragma once
#include <string>

#include "../Math/Vector3.h"
#include "Color.h"

class Texture {
  ImageColor* data;
  int width, height, comp;

 public:
  Color sample(Vector3f uv);
  void loadFromFile(std::string filename);
  ~Texture();
};
