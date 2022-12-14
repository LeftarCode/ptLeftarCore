#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <array>
#include <random>
#include <thread>

#include "Graphics/Color.h"
#include "Graphics/Material.h"
#include "Graphics/Primitives/Plane.h"
#include "Graphics/Primitives/Sphere.h"
#include "Graphics/Primitives/Triangle.h"
#include "Graphics/Spectrum/RGBSpectrum.h"
#include "Graphics/Spectrum/SampledSpectrum.h"
#include "Graphics/Techniques/BRDFTechnique.h"
#include "Math/Camera.h"
#include "Math/Ray.h"
#include "Optimizers/Octree/Octree.h"
#include "Externals/objloader.h"
#include "Externals/stb/stb_image_write.h"

const int ThreadsCount = 16;
ImageColor* data = nullptr;
std::vector<Material> materials;
std::vector<Triangle> triangles;
std::vector<Sphere> spheres;
std::vector<Plane> planes;

BRDFTechnique brdf;

Color secondaryRay(Ray& ray, Octree& octree, int depth) {
  if (depth <= 0) {
    return Color(0, 0, 0);
  }

  Primitive::HitDescriptor hit;
  if (octree.hit(ray, hit)) {
    Color result{1, 1, 1};

    Ray newRay;
    newRay.origin = hit.position;
    newRay.direction = hit.normal + Vector3f::randomInHemisphere(hit.normal);
    if (hit.materialId >= 0) {
      Material mat = materials[hit.materialId];
      result = mat.diffuseColor;

      if (mat.type == eReflective) {
        brdf.scatter(hit, ray.direction, newRay.direction);
        newRay.direction =
            newRay.direction +
            Vector3f::randomInHemisphere(hit.normal) * mat.roughness;
      }
    }

    newRay.direction.normalize();
    newRay.invDirection =
        Vector3f(1.0f / newRay.direction.x, 1.0f / newRay.direction.y,
                 1.0f / newRay.direction.z);
    newRay.invDirection.normalize();

    result *= secondaryRay(newRay, octree, depth - 1) * 0.5;
    return result;
  }

  Vector3f unitDirection = ray.direction;
  unitDirection.normalize();
  float t = 0.5 * (unitDirection.y + 1.0);
  return Color(1, 1, 1) * (1.0 - t) + Color(0.5, 0.7, 1.0) * t;
}

void render(int width,
            int height,
            Camera& camera,
            Octree& octree,
            int workerID) {
  int partialWidth = width / ThreadsCount;
  for (int i = partialWidth * workerID;
       i < partialWidth * workerID + partialWidth; i++) {
    for (int j = 0; j < height; j++) {
      Ray ray = camera.castRay(i, j);

      Color result = {0, 0, 0};
      for (int i = 0; i < 512; i++) {
        result += secondaryRay(ray, octree, 12);
      }
      result /= 512;

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

  Sphere sphere(Vector3f(0, -1, -4), 3, materials.size() - 1);
  spheres.push_back(sphere);

  Material reflectiveSphere;
  reflectiveSphere.diffuseColor = Color{0, 1, 0};
  reflectiveSphere.type = eReflective;
  reflectiveSphere.roughness = 0.5;
  materials.push_back(reflectiveSphere);
  Sphere sphere1(Vector3f(0, -1, 3), 3, materials.size() - 1);
  spheres.push_back(sphere1);

  Material lightMat;
  lightMat.diffuseColor = Color{1, 0, 0};
  lightMat.type = eReflective;
  lightMat.roughness = 0.0f;
  materials.push_back(lightMat);
  Sphere light(Vector3f(0, 8, 4), 4, materials.size() - 1);
  spheres.push_back(light);

  Material reflectivePlane;
  reflectivePlane.diffuseColor = Color{1, 1, 1};
  reflectivePlane.type = eReflective;
  reflectivePlane.roughness = 0.7;
  materials.push_back(reflectivePlane);
  Plane plane(Vector3f(0, -4, 0), Vector3f(0, 1, 0), materials.size() - 1);
  planes.push_back(plane);

  Octree octree(triangles, spheres, planes);
  Camera camera(Vector3f(-12, 1, 0), width, height, 45.0f);
  camera.lookAt(camera.origin + Vector3f(1, 0, 0));

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
