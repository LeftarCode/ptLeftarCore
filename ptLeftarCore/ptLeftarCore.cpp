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

const int ThreadsCount = 16;
Color *data = nullptr;

void render(int width, int height, Camera& camera, Octree& octree, int workerID) {
  int partialWidth = width / ThreadsCount;
  for (int i = partialWidth * workerID; i < partialWidth * workerID + partialWidth; i++) {
    for (int j = 0; j < height; j++) {
      Ray r = camera.castRay(i, j);
      Primitive::HitDescriptor hit;

      if (octree.hit(r, hit)) {
        hit.normal.normalize();

        Vector3f lightDirection = Vector3f(0.0f, -1.0f, -1.0f);
        lightDirection.normalize();
        float intense = lightDirection.dotProduct(hit.normal);
        if (intense < 0) {
          intense = 0;
        } else if (intense > 1) {
          intense = 1;
        }

        data[j * width + i].r = fabs(hit.normal.x) * 255;
        data[j * width + i].g = fabs(hit.normal.y) * 255;
        data[j * width + i].b = fabs(hit.normal.z) * 255;
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
  bool succes = Loader.LoadFile("sponza/sponza.obj");
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

    
		for (int j = 0; j < curMesh.Indices.size(); j += 3) {
      Triangle triangle = Triangle(vertices[curMesh.Indices[j]],
                        vertices[curMesh.Indices[j + 1]],
                        vertices[curMesh.Indices[j + 2]]);

      modelAABB.extend(vertices[curMesh.Indices[j]].position);
      modelAABB.extend(vertices[curMesh.Indices[j + 1]].position);
      modelAABB.extend(vertices[curMesh.Indices[j + 2]].position);
      triangles.push_back(triangle);
    }
  }

  Octree octree(triangles);
  //camera.lookAt(modelAABB.center);
  Camera camera(modelAABB.center + Vector3f(0,0,-5), width, height,
                45.0f);
  camera.lookAt(camera.origin + Vector3f(1,0,0));
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
