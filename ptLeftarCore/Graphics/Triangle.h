#pragma once
#include "Primitive.h"
#include "Color.h"
#include "../Math/Ray.h"
#include "Vertex.h"
#include "AABB.h"
#include "../Math/Base.h"
#include <array>

class Triangle : public Primitive {
public:
  Vertex v1, v2, v3;
  int materialId;

public:
  Triangle(Vertex v1, Vertex v2, Vertex v3);
  virtual bool hit(const Ray &ray, Primitive::HitDescriptor& hitDescriptor);
  Primitive::HitDescriptor getHitDescriptorFromPoint(Vector3f point);
  AABB getBoundingBox() const;
};

class PackedTriangles {
public:
  __m256 e1[3];
  __m256 e2[3];
  __m256 v1[3];

  std::array<Triangle *, 8> triangles;
  bool __vectorcall avx_intersection(const PackedRay& packedRays,
                        PackedIntersectionResult &result,
                        Primitive::HitDescriptor &hitDescriptor) {

    __m256 q[3];
    avx_multi_cross(q, packedRays.m_direction, e2);

    __m256 a = avx_multi_dot(e1, q);
    __m256 f = _mm256_div_ps(IdentityM256, a);

    __m256 s[3];
    avx_multi_sub(s, packedRays.m_origin, v1);

    __m256 u = _mm256_mul_ps(f, avx_multi_dot(s, q));

    __m256 r[3];
    avx_multi_cross(r, s, e1);

    __m256 v = _mm256_mul_ps(f, avx_multi_dot(packedRays.m_direction, r));
    __m256 t = _mm256_mul_ps(f, avx_multi_dot(e2, r));

    __m256 failed =
        _mm256_and_ps(_mm256_cmp_ps(a, NegativeEpsilonM256, _CMP_GT_OQ),
                      _mm256_cmp_ps(a, PositiveEpsilonM256, _CMP_LT_OQ));

    failed = _mm256_or_ps(failed, _mm256_cmp_ps(u, ZeroM256, _CMP_LT_OQ));
    failed = _mm256_or_ps(failed, _mm256_cmp_ps(v, ZeroM256, _CMP_LT_OQ));
    failed = _mm256_or_ps(failed, _mm256_cmp_ps(_mm256_add_ps(u, v), IdentityM256, _CMP_GT_OQ));
    failed = _mm256_or_ps(failed, _mm256_cmp_ps(t, ZeroM256, _CMP_LT_OQ));

    __m256 tResults = _mm256_blendv_ps(t, MinusIdentityM256, failed);

    int mask = _mm256_movemask_ps(tResults);
    if (mask != 0xFF) {
      result.idx = -1;

      float *ptr = (float *)&tResults;
      for (int i = 0; i < 8; ++i) {
        if (ptr[i] >= 0.0f && ptr[i] < result.t) {
          result.t = ptr[i];
          result.idx = i;
        }
      }
      if (result.idx != -1) {
        Ray r;
        r.direction.x = packedRays.m_direction[0].m256_f32[0];
        r.direction.y = packedRays.m_direction[1].m256_f32[0];
        r.direction.z = packedRays.m_direction[2].m256_f32[0];
        r.origin.x = packedRays.m_origin[0].m256_f32[0];
        r.origin.y = packedRays.m_origin[1].m256_f32[0];
        r.origin.z = packedRays.m_origin[2].m256_f32[0];

        Triangle *triangle = triangles[result.idx];
        Vector3f point = r.origin + r.direction * result.t;

        hitDescriptor = triangle->getHitDescriptorFromPoint(point);
        return true;
      }
    }

    return false;
  }
};