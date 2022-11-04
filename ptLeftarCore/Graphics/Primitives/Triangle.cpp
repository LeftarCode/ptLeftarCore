#include "Triangle.h"

#include <algorithm>

static float AreaOfTriangle(float a, float b, float c) {
  float p = (a + b + c) / 2.0f;
  float area_sqr = p * (p - a) * (p - b) * (p - c);

  if (area_sqr < 0.0) {
    return 0.0;
  }

  return sqrtf(area_sqr);
}

Triangle::Triangle(Vertex v1, Vertex v2, Vertex v3)
    : Primitive(PrimitiveType::eTriangle), v1(v1), v2(v2), v3(v3) {
  Vector3f e1 = v2.position - v1.position;
  Vector3f e2 = v3.position - v1.position;
}

Triangle::Triangle() : Primitive(PrimitiveType::eTriangle) {}

bool Triangle::hit(const Ray& ray, Primitive::HitDescriptor& hitDescriptor) {
  BoundingBox aabb = getBoundingBox();
  Vector3f dirfrac = ray.invDirection;

  float t1 = (aabb.min.x - ray.origin.x) * dirfrac.x;
  float t2 = (aabb.max.x - ray.origin.x) * dirfrac.x;
  float t3 = (aabb.min.y - ray.origin.y) * dirfrac.y;
  float t4 = (aabb.max.y - ray.origin.y) * dirfrac.y;
  float t5 = (aabb.min.z - ray.origin.z) * dirfrac.z;
  float t6 = (aabb.max.z - ray.origin.z) * dirfrac.z;

  float tmax = std::min({std::max(t1, t2), std::max(t3, t4), std::max(t5, t6)});
  if (tmax < 0.0) {
    return false;
  }

  float tmin = std::max({std::min(t1, t2), std::min(t3, t4), std::min(t5, t6)});
  if (tmin > tmax) {
    return false;
  }

  Vector3f e1 = v2.position - v1.position;
  Vector3f e2 = v3.position - v1.position;
  Vector3f pvec = ray.direction.crossProduct(e2);
  float det = e1.dotProduct(pvec);

  if (fabs(det) < 0.000000001) {
    return false;
  }

  float inv_det = 1.0f / det;
  Vector3f tvec = ray.origin - v1.position;
  float u = tvec.dotProduct(pvec) * inv_det;
  if (u < 0.0 || u > 1.0) {
    return false;
  }

  Vector3f qvec = tvec.crossProduct(e1);
  float v = ray.direction.dotProduct(qvec) * inv_det;
  if (v < 0.0 || u + v > 1.0) {
    return false;
  }

  float final_distance = e2.dotProduct(qvec) * inv_det;
  if (final_distance < 0.0) {
    return false;
  }

  Vector3f point = ray.origin + ray.direction * final_distance;

  float a = v1.position.distance(v2.position);
  float b = v2.position.distance(v3.position);
  float c = v3.position.distance(v1.position);

  float p0 = point.distance(v1.position);
  float p1 = point.distance(v2.position);
  float p2 = point.distance(v3.position);

  float n0 = AreaOfTriangle(b, p2, p1);
  float n1 = AreaOfTriangle(c, p0, p2);
  float n2 = AreaOfTriangle(a, p1, p0);

  float n = n0 + n1 + n2;

  hitDescriptor.position = point;
  hitDescriptor.normal = (v1.normal * n0 + v2.normal * n1 + v3.normal * n2) / n;
  hitDescriptor.uv = (v1.uv * n0 + v2.uv * n1 + v3.uv * n2) / n;
  hitDescriptor.primitive = this;
  hitDescriptor.materialId = materialId;

  return true;
}

Primitive::HitDescriptor Triangle::getHitDescriptorFromPoint(Vector3f point) {
  float a = v1.position.distance(v2.position);
  float b = v2.position.distance(v3.position);
  float c = v3.position.distance(v1.position);

  float p0 = point.distance(v1.position);
  float p1 = point.distance(v2.position);
  float p2 = point.distance(v3.position);

  float n0 = AreaOfTriangle(b, p2, p1);
  float n1 = AreaOfTriangle(c, p0, p2);
  float n2 = AreaOfTriangle(a, p1, p0);

  float n = n0 + n1 + n2;

  Primitive::HitDescriptor hitDescriptor;
  hitDescriptor.position = point;
  hitDescriptor.normal = (v1.normal * n0 + v2.normal * n1 + v3.normal * n2) / n;
  hitDescriptor.uv = (v1.uv * n0 + v2.uv * n1 + v3.uv * n2) / n;
  hitDescriptor.primitive = this;
  hitDescriptor.materialId = materialId;

  return hitDescriptor;
}

BoundingBox Triangle::getBoundingBox() const {
  BoundingBox aabb(v1.position, v1.position);
  aabb.extend(v2.position);
  aabb.extend(v3.position);

  return aabb;
}

bool __vectorcall PackedTriangles::hit(
    const PackedRay& packedRays,
    PackedIntersectionResult& result,
    Primitive::HitDescriptor& hitDescriptor) {
  __m256 q[3];
  multiCross(q, packedRays.direction, e2);

  __m256 a = multiDot(e1, q);
  __m256 f = _mm256_div_ps(IdentityM256, a);

  __m256 s[3];
  multiSub(s, packedRays.origin, v1);

  __m256 u = _mm256_mul_ps(f, multiDot(s, q));

  __m256 r[3];
  multiCross(r, s, e1);

  __m256 v = _mm256_mul_ps(f, multiDot(packedRays.direction, r));
  __m256 t = _mm256_mul_ps(f, multiDot(e2, r));

  __m256 failed =
      _mm256_and_ps(_mm256_cmp_ps(a, NegativeEpsilonM256, _CMP_GT_OQ),
                    _mm256_cmp_ps(a, PositiveEpsilonM256, _CMP_LT_OQ));

  failed = _mm256_or_ps(failed, _mm256_cmp_ps(u, ZeroM256, _CMP_LT_OQ));
  failed = _mm256_or_ps(failed, _mm256_cmp_ps(v, ZeroM256, _CMP_LT_OQ));
  failed = _mm256_or_ps(
      failed, _mm256_cmp_ps(_mm256_add_ps(u, v), IdentityM256, _CMP_GT_OQ));
  failed = _mm256_or_ps(failed, _mm256_cmp_ps(t, ZeroM256, _CMP_LT_OQ));

  __m256 tResults = _mm256_blendv_ps(t, MinusIdentityM256, failed);

  int mask = _mm256_movemask_ps(tResults);
  if (mask != 0xFF) {
    result.idx = -1;

    float* ptr = (float*)&tResults;
    for (int i = 0; i < 8; ++i) {
      if (ptr[i] >= 0.0f && ptr[i] < result.t) {
        result.t = ptr[i];
        result.idx = i;
      }
    }
    if (result.idx != -1) {
      Ray r;
      r.direction.x = packedRays.direction[0].m256_f32[0];
      r.direction.y = packedRays.direction[1].m256_f32[0];
      r.direction.z = packedRays.direction[2].m256_f32[0];
      r.origin.x = packedRays.origin[0].m256_f32[0];
      r.origin.y = packedRays.origin[1].m256_f32[0];
      r.origin.z = packedRays.origin[2].m256_f32[0];

      Triangle* triangle = triangles[result.idx];
      Vector3f point = r.origin + r.direction * result.t;

      hitDescriptor = triangle->getHitDescriptorFromPoint(point);
      return true;
    }
  }

  return false;
}
