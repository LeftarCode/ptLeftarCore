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
#include <array>

const int ThreadsCount = 16;
Color *data = nullptr;
std::vector<Material> materials;
std::vector<Triangle> triangles;
std::vector<PackedTriangles> packedTriangles;

void render(int width, int height, Camera& camera, Octree& octree, int workerID) {
  int partialWidth = width / ThreadsCount;
  for (int i = partialWidth * workerID; i < partialWidth * workerID + partialWidth; i++) {
    for (int j = 0; j < height; j++) {
      Ray r = camera.castRay(i, j);

      PackedRay packedRay;
      for (int k = 0; k < 8; k++) {
        packedRay.m_direction[0].m256_f32[k] = r.direction.x;
        packedRay.m_direction[1].m256_f32[k] = r.direction.y;
        packedRay.m_direction[2].m256_f32[k] = r.direction.z;

        packedRay.m_origin[0].m256_f32[k] = r.origin.x;
        packedRay.m_origin[1].m256_f32[k] = r.origin.y;
        packedRay.m_origin[2].m256_f32[k] = r.origin.z;
      }
      
      Primitive::HitDescriptor closestHit;
      float closestDistance = std::numeric_limits<float>::max();
      for (auto pack : packedTriangles) {
        PackedIntersectionResult result;
        Primitive::HitDescriptor hitDesc;
        if (pack.avx_intersection(packedRay, result, hitDesc)) {
          int idx = result.idx;

          float distance = hitDesc.position.distance(r.origin);
          if (distance < closestDistance) {
            closestDistance = distance;
            closestHit = hitDesc;
          }
        }
      }

      Vector3f lightDirection = Vector3f(0.0f, -1.0f, -1.0f);
      lightDirection.normalize();
      float intense = lightDirection.dotProduct(closestHit.normal);
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

int main() {
  int width = 160;
  int height = 160;
  data = (Color *)malloc(sizeof(Color) * width * height);
  memset(data, 0, sizeof(Color) * width * height);

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

  const int X = 0;
  const int Y = 1;
  const int Z = 2;

  int trianglesCount = triangles.size();
  for (int i = 0; i < trianglesCount; i += 8) {
    PackedTriangles packed;
    for (int j = 0; j < 8; j++) {
      Vector3f e1 = triangles[(i + j) % trianglesCount].v2.position -
                    triangles[(i + j) % trianglesCount].v1.position;
      Vector3f e2 = triangles[(i + j) % trianglesCount].v3.position -
                    triangles[(i + j) % trianglesCount].v1.position;
      Vector3f v1 = triangles[(i + j) % trianglesCount].v1.position;

      packed.e1[X].m256_f32[j] = e1.x;
      packed.e1[Y].m256_f32[j] = e1.y;
      packed.e1[Z].m256_f32[j] = e1.z;
      packed.e2[X].m256_f32[j] = e2.x;
      packed.e2[Y].m256_f32[j] = e2.y;
      packed.e2[Z].m256_f32[j] = e2.z;
      packed.v1[X].m256_f32[j] = v1.x;
      packed.v1[Y].m256_f32[j] = v1.y;
      packed.v1[Z].m256_f32[j] = v1.z;
      packed.triangles[j] = &triangles[(i + j) % trianglesCount];
    }
    packedTriangles.push_back(packed);
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
  free(data);

	return 0;
}
