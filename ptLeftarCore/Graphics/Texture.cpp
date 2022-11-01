#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stbi/stb_image.h"

Color Texture::sample(Vector3f uv) {
  int coordX = uv.x * width;
  int coordY = uv.y * height;
  
  return (Color)data[coordY * width + coordX];
}

void Texture::loadFromFile(std::string filename) {
  data = (Color*)stbi_load(filename.c_str(), &width, &height, &comp, 3);
}

Texture::~Texture() { 
  stbi_image_free(data);
}