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

const int ThreadsCount = 16;
Color *data = nullptr;
std::vector<Material> materials;

void render(int width, int height, Camera& camera, Octree& octree, int workerID) {
  int partialWidth = width / ThreadsCount;
  for (int i = partialWidth * workerID; i < partialWidth * workerID + partialWidth; i++) {
    for (int j = 0; j < height; j++) {
      Ray r = camera.castRay(i, j);
      Primitive::HitDescriptor hit;

      if (octree.hit(r, hit)) {
        hit.normal.normalize();

        Material currentMaterial = materials[hit.materialId];

        Vector3f lightDirection = Vector3f(0.0f, -1.0f, -1.0f);
        lightDirection.normalize();
        float intense = lightDirection.dotProduct(hit.normal);
        if (intense < 0) {
          intense = 0;
        } else if (intense > 1) {
          intense = 1;
        }

        data[j * width + i] = currentMaterial.diffuseTexture->sample(hit.uv);
      }
    }
  }
}

int main() {
  int width = 400;
  int height = 400;
  data = (Color *)malloc(sizeof(Color) * width * height);
  memset(data, 0, sizeof(Color) * width * height);
  std::vector<Triangle> triangles;

  AABB modelAABB(Vector3f(0, 0, 0), Vector3f(0, 0, 0));
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
    material.diffuseColor.r = curMesh.MeshMaterial.Kd.X * 255;
    material.diffuseColor.g = curMesh.MeshMaterial.Kd.Y * 255;
    material.diffuseColor.b = curMesh.MeshMaterial.Kd.Z * 255;
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

  Octree octree(triangles);
  //camera.lookAt(modelAABB.center);
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

	return 0;
}
