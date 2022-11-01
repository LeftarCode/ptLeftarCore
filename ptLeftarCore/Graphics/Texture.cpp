#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stbi/stb_image.h"

Color Texture::sample(Vector3f uv) {
  uv.x = fmod(uv.x, 1.0);
  uv.y = fmod(uv.y, 1.0);

  if (uv.x < 0.0) {
    uv.x += 1.0f;
  }

  if (uv.y < 0.0) {
    uv.y += 1.0f;
  }

  int coordX = uv.x * (width - 1);
  int coordY = uv.y * (height - 1);

  if (data != nullptr) {
    return (Color)data[coordY * width + coordX];
  }

  return Color{0, 0, 0};
}

void Texture::loadFromFile(std::string filename) {
    data = (Color *)stbi_load(filename.c_str(), &width, &height, &comp, STBI_rgb);
}

Texture::~Texture() { 
  stbi_image_free(data);
}