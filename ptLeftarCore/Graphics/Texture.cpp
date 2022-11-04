#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stbi/stb_image.h"

Color Texture::sample(Vector3f uv) {
  uv.x = fmodf(uv.x, 1.0);
  uv.y = fmodf(uv.y, 1.0);

  if (uv.x < 0.0) {
    uv.x += 1.0f;
  }

  if (uv.y < 0.0) {
    uv.y += 1.0f;
  }

  int coordX = uv.x * (width - 1);
  int coordY = uv.y * (height - 1);

  if (data != nullptr) {
    ImageColor color = data[coordY * width + coordX];
    Color out;
    out.r = color.r / 255.0f;
    out.g = color.g / 255.0f;
    out.b = color.b / 255.0f;

    return out;
  }

  return Color{0, 0, 0};
}

void Texture::loadFromFile(std::string filename) {
  data = (ImageColor *)stbi_load(filename.c_str(), &width, &height, &comp,
                                 STBI_rgb);
}

Texture::~Texture() { 
  stbi_image_free(data);
}