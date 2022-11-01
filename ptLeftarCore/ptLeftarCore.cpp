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

Color *data = nullptr;


void render(int width, int height, Camera& camera, Octree& octree, int workerID) {
  int partialWidth = width / 8;
}

int main() {
  int width = 1920;
  int height = 1080;
  data = (Color *)malloc(sizeof(Color) * width * height);
  memset(data, 0, sizeof(Color) * width * height);

  // Camera
  Camera camera({0.0f, 0.0f, 10.0f}, width, height);

  std::vector<Vertex> vertices;
  std::vector<Triangle> triangles;

  objl::Loader Loader;
  bool succes = Loader.LoadFile("monkey.obj");
  for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
    objl::Mesh curMesh = Loader.LoadedMeshes[i];
    for (int j = 0; j < curMesh.Vertices.size(); j++) {
      Vertex vertex;
      vertex.position = Vector3f(curMesh.Vertices[j].Position.X,
                                 curMesh.Vertices[j].Position.Y,
                                 curMesh.Vertices[j].Position.Z+1);
      vertex.normal = Vector3f(curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y,
                   curMesh.Vertices[j].Normal.Z);

      vertices.push_back(vertex);
    }

    
		for (int j = 0; j < curMesh.Indices.size(); j += 3) {
      Triangle triangle(vertices[curMesh.Indices[j]],
                        vertices[curMesh.Indices[j + 1]],
                        vertices[curMesh.Indices[j + 2]]);

      triangles.push_back(triangle);
    }
  }

  Octree octree(triangles);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      Ray r = camera.castRay(i, j);

      Vector3f barycentricCoords;
      Vector3f normal;
      if (octree.hit(r, normal)) {
        normal.normalize();

        Vector3f lightDirection = Vector3f(0.0f, 0.0f, -1.0f);
        lightDirection.normalize();
        float intense = lightDirection.dotProduct(normal);
        if (intense < 0) {
          intense = 0;
        } else if (intense > 1) {
          intense = 1;
        }

        data[j * width + i].r = 255 * intense;
        data[j * width + i].g = 255 * intense;
        data[j * width + i].b = 255 * intense;
      }
    }
  }

  stbi_write_png("render.png", width, height, 3, data, 0);

	return 0;
}
