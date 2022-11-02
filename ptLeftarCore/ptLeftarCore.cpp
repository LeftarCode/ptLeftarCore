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

struct PackedRay {
  __m256 m_origin[3];
  __m256 m_direction[3];
};

struct PackedIntersectionResult {
  float t = std::numeric_limits<float>::max();
  int idx;
};

struct PackedTriangles {
  __m256 e1[3];
  __m256 e2[3];
  __m256 v1[3];
  std::array<int, 8> trianglesIds;
};

const int ThreadsCount = 16;
Color *data = nullptr;
std::vector<Material> materials;
std::vector<Triangle> triangles;
std::vector<PackedTriangles> packedTriangles;


void avx_multi_cross(__m256 result[3], const __m256 a[3], const __m256 b[3]) {
  result[0] = _mm256_fmsub_ps(a[1], b[2], _mm256_mul_ps(b[1], a[2]));
  result[1] = _mm256_fmsub_ps(a[2], b[0], _mm256_mul_ps(b[2], a[0]));
  result[2] = _mm256_fmsub_ps(a[0], b[1], _mm256_mul_ps(b[0], a[1]));
}

__m256 avx_multi_dot(const __m256 a[3], const __m256 b[3]) {
  return _mm256_fmadd_ps(a[2], b[2], _mm256_fmadd_ps(a[1], b[1], _mm256_mul_ps(a[0], b[0])));
}

void avx_multi_sub(__m256 result[3], const __m256 a[3], const __m256 b[3]) {
  result[0] = _mm256_sub_ps(a[0], b[0]);
  result[1] = _mm256_sub_ps(a[1], b[1]);
  result[2] = _mm256_sub_ps(a[2], b[2]);
}

const __m256 oneM256 = _mm256_set1_ps(1.0f);
const __m256 minusOneM256 = _mm256_set1_ps(-1.0f);
const __m256 positiveEpsilonM256 = _mm256_set1_ps(1e-6f);
const __m256 negativeEpsilonM256 = _mm256_set1_ps(-1e-6f);
const __m256 zeroM256 = _mm256_set1_ps(0.0f);

bool avx_intersection(const PackedTriangles &packedTris,
                      const PackedRay packedRays,
                      PackedIntersectionResult &result) {

  __m256 q[3];
  avx_multi_cross(q, packedRays.m_direction, packedTris.e2);

  __m256 a = avx_multi_dot(packedTris.e1, q);
  __m256 f = _mm256_div_ps(oneM256, a);

  __m256 s[3];
  avx_multi_sub(s, packedRays.m_origin, packedTris.v1);

  __m256 u = _mm256_mul_ps(f, avx_multi_dot(s, q));

  __m256 r[3];
  avx_multi_cross(r, s, packedTris.e1);

  __m256 v = _mm256_mul_ps(f, avx_multi_dot(packedRays.m_direction, r));
  __m256 t = _mm256_mul_ps(f, avx_multi_dot(packedTris.e2, r));

  // Failure conditions
  __m256 failed =
      _mm256_and_ps(_mm256_cmp_ps(a, negativeEpsilonM256, _CMP_GT_OQ),
                    _mm256_cmp_ps(a, positiveEpsilonM256, _CMP_LT_OQ));

  failed = _mm256_or_ps(failed, _mm256_cmp_ps(u, zeroM256, _CMP_LT_OQ));
  failed = _mm256_or_ps(failed, _mm256_cmp_ps(v, zeroM256, _CMP_LT_OQ));
  failed = _mm256_or_ps(
      failed, _mm256_cmp_ps(_mm256_add_ps(u, v), oneM256, _CMP_GT_OQ));
  failed = _mm256_or_ps(failed, _mm256_cmp_ps(t, zeroM256, _CMP_LT_OQ));

  __m256 tResults = _mm256_blendv_ps(t, minusOneM256, failed);

  int mask = _mm256_movemask_ps(tResults);
  if (mask != 0xFF) {
    // There is at least one intersection
    result.idx = -1;

    float *ptr = (float *)&tResults;
    for (int i = 0; i < 8; ++i) {
      if (ptr[i] >= 0.0f && ptr[i] < result.t) {
        result.t = ptr[i];
        result.idx = i;
      }
    }

    return result.idx != -1;
  }

  return false;
}

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
        if (avx_intersection(pack, packedRay, result)) {
          int triangleIdxInPackage = result.idx;
          Triangle hitTriangle =
              triangles[pack.trianglesIds[triangleIdxInPackage]];
          Vector3f point = r.origin + r.direction * result.t; 

          Primitive::HitDescriptor hitDesc =
              hitTriangle.getHitDescriptorFromPoint(point);
          hitDesc.normal.normalize();

          float distance = point.distance(r.origin);
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

      /*for (Triangle triangle : triangles) {
        if (triangle.hit(r, hit)) {
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

          data[j * width + i].r = 255 * intense;
          data[j * width + i].g = 255 * intense;
          data[j * width + i].b = 255 * intense;
        }
      }*/
    }
  }
}

int main() {
  int width = 640;
  int height = 640;
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
      packed.trianglesIds[j] = (i + j) % trianglesCount;
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
