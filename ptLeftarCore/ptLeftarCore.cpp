#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stbi/stb_image_write.h"
#include "Graphics/Color.h"
#include "Math/Ray.h"
#include "Graphics/Triangle.h"
#include "Math/Camera.h"
#include "../external/objloader.h"
#include "Graphics/Octree.h"
#include <thread>
#include "Graphics/Material.h"
#include "Graphics/Sphere.h"
#include <array>
#include <random>

const int ThreadsCount = 16;
ImageColor *data = nullptr;
std::vector<Material> materials;
std::vector<Triangle> triangles;
std::vector<Sphere> spheres;

Color secondaryRay(Ray &ray, Octree &octree, int depth) {
  if (depth <= 0) {
    return Color{1, 1, 1};
  }

  Primitive::HitDescriptor hit;
  if (octree.hit(ray, hit)) {
    Vector3f target =
        hit.position + hit.normal + Vector3f::randomInHemisphere(hit.normal);
    Vector3f newPostion = hit.position;
    Vector3f newDirection = target - hit.position;
    newDirection.normalize();

    Ray newRay;
    newRay.origin = newPostion;
    newRay.direction = newDirection;
    newRay.direction.normalize();
    newRay.invDirection = Vector3f(1.0f / newRay.direction.x, 1.0f / newRay.direction.y,
                 1.0f / newRay.direction.z);
    newRay.invDirection.normalize();

    Vector3f lightDirection = Vector3f(0.0f, -1.0f, -1.0f);
    lightDirection.normalize();

    Material currMat = materials[hit.materialId];
    Color diffuseColor = currMat.diffuseColor;
    if (currMat.diffuseTexture != nullptr) {
      diffuseColor = currMat.diffuseTexture->sample(hit.uv);
    }

    return diffuseColor * secondaryRay(newRay, octree, depth - 1);
  }

  return Color{0, 0, 0};
}

void render(int width, int height, Camera& camera, Octree& octree, int workerID) {
  int partialWidth = width / ThreadsCount;
  for (int i = partialWidth * workerID;
       i < partialWidth * workerID + partialWidth; i++) {
    for (int j = 0; j < height; j++) {
      Ray r = camera.castRay(i, j);

      Color result{0, 0, 0}; 
      for (int sample = 0; sample < 64; sample++) {
        result += secondaryRay(r, octree, 4);
      }
      result /= 64;
      ImageColor color;
      color.r = result.r * 255;
      color.g = result.g * 255;
      color.b = result.b * 255;
      data[j * width + i] = color;
    }
  }
}

int main() {
  int width = 160;
  int height = 160;
  data = (ImageColor *)malloc(sizeof(ImageColor) * width * height);
  memset(data, 0, sizeof(ImageColor) * width * height);

  BoundingBox modelAABB(Vector3f(0, 0, 0), Vector3f(0, 0, 0));
  objl::Loader Loader;
  bool succes = Loader.LoadFile("crytek-sponza-huge-vray-obj/crytek-sponza-huge-vray.obj");
  for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
    objl::Mesh curMesh = Loader.LoadedMeshes[i];
    std::vector<Vertex> vertices;
    for (int j = 0; j < curMesh.Vertices.size(); j++) {
      Vertex vertex;
      vertex.position = Vector3f(curMesh.Vertices[j].Position.X,
                                 curMesh.Vertices[j].Position.Y,
                                 curMesh.Vertices[j].Position.Z);
      vertex.normal = Vector3f(curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y,
                   curMesh.Vertices[j].Normal.Z);
      vertex.uv = Vector3f(curMesh.Vertices[j].TextureCoordinate.X,
                           curMesh.Vertices[j].TextureCoordinate.Y,
                           0.0f);

      vertices.push_back(vertex);
    }

    Material material;
    material.diffuseColor.r = curMesh.MeshMaterial.Kd.X;
    material.diffuseColor.g = curMesh.MeshMaterial.Kd.Y;
    material.diffuseColor.b = curMesh.MeshMaterial.Kd.Z;
    if (curMesh.MeshMaterial.map_Kd.length() > 0) {
      Texture *diffuse = new Texture();
      diffuse->loadFromFile("crytek-sponza-huge-vray-obj\\" + curMesh.MeshMaterial.map_Kd);
      material.diffuseTexture = diffuse;
    }
    materials.push_back(material);
    
		for (int j = 0; j < curMesh.Indices.size(); j += 3) {
      Triangle triangle = Triangle(vertices[curMesh.Indices[j]],
                        vertices[curMesh.Indices[j + 1]],
                        vertices[curMesh.Indices[j + 2]]);
      triangle.materialId = materials.size() - 1;

      modelAABB.extend(vertices[curMesh.Indices[j]].position);
      modelAABB.extend(vertices[curMesh.Indices[j + 1]].position);
      modelAABB.extend(vertices[curMesh.Indices[j + 2]].position);
      triangles.push_back(triangle);
    }
  }

  Material white;
  white.diffuseColor = Color{1, 1, 1};
  white.diffuseTexture = nullptr;
  materials.push_back(white);

  Sphere sphere(modelAABB.center + Vector3f(-230, -400, 0), 50, materials.size() - 1);
  //spheres.push_back(sphere);

  Octree octree(triangles, spheres);
  Camera camera(modelAABB.center + Vector3f(0,-300,0), width, height,
                45.0f);
  camera.lookAt(camera.origin + Vector3f(-1,0,0));
  std::vector<std::thread> threads;
  threads.reserve(ThreadsCount);
  for (int i = 0; i < ThreadsCount; i++) {
    threads.push_back(std::thread(render, width, height, std::ref(camera),
                                  std::ref(octree), i));
  }

  for (int i = 0; i < ThreadsCount; i++) {
    threads[i].join();
  }

  printf("RENDERED!\n");
  stbi_write_png("render.png", width, height, 3, data, 0);
  free(data);

	return 0;
}
