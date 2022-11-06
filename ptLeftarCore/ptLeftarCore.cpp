#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <array>
#include <random>
#include <thread>

#include "../external/objloader.h"
#include "../external/stbi/stb_image_write.h"
#include "Graphics/Color.h"
#include "Graphics/Material.h"
#include "Graphics/Primitives/Sphere.h"
#include "Graphics/Primitives/Triangle.h"
#include "Graphics/Spectrum/RGBSpectrum.h"
#include "Graphics/Spectrum/SampledSpectrum.h"
#include "Math/Camera.h"
#include "Math/Ray.h"
#include "Optimizers/Octree/Octree.h"

const int ThreadsCount = 16;
ImageColor* data = nullptr;
std::vector<Material> materials;
std::vector<Triangle> triangles;
std::vector<Sphere> spheres;

void render(int width,
            int height,
            Camera& camera,
            Octree& octree,
            int workerID) {
  int partialWidth = width / ThreadsCount;
  for (int i = partialWidth * workerID;
       i < partialWidth * workerID + partialWidth; i++) {
    for (int j = 0; j < height; j++) {
      Ray r = camera.castRay(i, j);

      Color result{0, 0, 0};
      Primitive::HitDescriptor hit;
      if (octree.hit(r, hit)) {
        Material currMat = materials[hit.materialId];
        result = currMat.diffuseColor;
      } else {
        Vector3f unitDirection = r.direction;
        unitDirection.normalize();
        float t = 0.5 * (unitDirection.y + 1.0);
        result = Color(0, 0, 0) * (1.0 - t) + Color(0.5, 0.7, 1.0) * t;
      }

      ImageColor color;
      color.r = result.r * 255;
      color.g = result.g * 255;
      color.b = result.b * 255;
      data[j * width + i] = color;
    }
  }
}

int main() {
  int width = 800;
  int height = 400;
  data = (ImageColor*)malloc(sizeof(ImageColor) * width * height);
  memset(data, 0, sizeof(ImageColor) * width * height);

  Material white;
  white.diffuseColor = Color{1, 1, 1};
  white.diffuseTexture = nullptr;
  materials.push_back(white);

  Sphere sphere(Vector3f(0, 0, 0), 50, materials.size() - 1);
  spheres.push_back(sphere);

  Octree octree(triangles, spheres);
  Camera camera(Vector3f(120, 0, 0), width, height, 45.0f);
  camera.lookAt(camera.origin + Vector3f(-1, 0, 0));

  std::vector<std::thread> threads;
  threads.reserve(ThreadsCount);
  for (int i = 0; i < ThreadsCount; i++) {
    threads.push_back(std::thread(render, width, height, std::ref(camera),
                                  std::ref(octree), i));
  }

  for (int i = 0; i < ThreadsCount; i++) {
    threads[i].join();
  }

  stbi_write_png("render.png", width, height, 3, data, 0);
  free(data);

  return 0;
}
