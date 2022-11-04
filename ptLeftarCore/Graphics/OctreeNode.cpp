#include "OctreeNode.h"
#include <algorithm>

#define TopLeftFront 0
#define TopRightFront 1
#define BottomRightFront 2
#define BottomLeftFront 3
#define TopLeftBottom 4
#define TopRightBottom 5
#define BottomRightBack 6
#define BottomLeftBack 7

OctreeNode::OctreeNode()
    : aabb(BoundingBox(Vector3f(), Vector3f())) {}

OctreeNode::OctreeNode(BoundingBox aabb)
    : aabb(aabb) {}

void OctreeNode::generatePackedTriangles() {
  int trianglesCount = triangles.size();
  if (trianglesCount > 0) {
    int packsCount = (trianglesCount / 8) + 1;
    for (int i = 0; i < packsCount; i++) {
      PackedTriangles packed;
      for (int j = 0; j < 8; j++) {
        int trgIdx = (i * 8 + j) % trianglesCount;
        Vector3f e1 =
            triangles[trgIdx].v2.position - triangles[trgIdx].v1.position;
        Vector3f e2 =
            triangles[trgIdx].v3.position - triangles[trgIdx].v1.position;
        Vector3f v1 = triangles[trgIdx].v1.position;

        packed.e1[0].m256_f32[j] = e1.x;
        packed.e1[1].m256_f32[j] = e1.y;
        packed.e1[2].m256_f32[j] = e1.z;
        packed.e2[0].m256_f32[j] = e2.x;
        packed.e2[1].m256_f32[j] = e2.y;
        packed.e2[2].m256_f32[j] = e2.z;
        packed.v1[0].m256_f32[j] = v1.x;
        packed.v1[1].m256_f32[j] = v1.y;
        packed.v1[2].m256_f32[j] = v1.z;
        packed.triangles[j] = &triangles[trgIdx];
      }
      packedTriangles.push_back(packed);
    }
  }
}

void OctreeNode::generateChildren() {

  Vector3f center = aabb.center;

  for (int i = 0; i < 8; i++) {
    BoundingBox nodeAABB;
    if (i == TopLeftFront) {
      Vector3f min = Vector3f(aabb.min.x, aabb.min.y, aabb.min.z);
      Vector3f max = Vector3f(center.x, center.y, center.z);
      nodeAABB = BoundingBox(min, max);
    } else if (i == TopRightFront) {
      Vector3f min(center.x, aabb.min.y, aabb.min.z);
      Vector3f max(aabb.max.x, center.y, center.z);
      nodeAABB = BoundingBox(min, max);
    } else if (i == BottomRightFront) {
      Vector3f min(aabb.min.x, aabb.min.y, center.z);
      Vector3f max(center.x, center.y, aabb.max.z);
      nodeAABB = BoundingBox(min, max);
    } else if (i == BottomLeftFront) {
      Vector3f min(center.x, aabb.min.y, center.z);
      Vector3f max(aabb.max.x, center.y, aabb.max.z);
      nodeAABB = BoundingBox(min, max);
    } else if (i == TopLeftBottom) {
      Vector3f min(aabb.min.x, center.y, aabb.min.z);
      Vector3f max(center.x, aabb.max.y, center.z);
      nodeAABB = BoundingBox(min, max);
    } else if (i == TopRightBottom) {
      Vector3f min(center.x, center.y, aabb.min.z);
      Vector3f max(aabb.max.x, aabb.max.y, center.z);
      nodeAABB = BoundingBox(min, max);
    } else if (i == BottomRightBack) {
      Vector3f min(aabb.min.x, center.y, center.z);
      Vector3f max(center.x, aabb.max.y, aabb.max.z);
      nodeAABB = BoundingBox(min, max);
    } else if (i == BottomLeftBack) {
      Vector3f min(center.x, center.y, center.z);
      Vector3f max(aabb.max.x, aabb.max.y, aabb.max.z);
      nodeAABB = BoundingBox(min, max);
    }

    children.push_back(new OctreeNode(nodeAABB));
  }
}

void OctreeNode::addTriangle(Triangle triangle) {
  triangles.push_back(triangle);
}

void OctreeNode::addSphere(Sphere sphere) { 
  spheres.push_back(sphere);
}

void OctreeNode::subdivide() { 
  if (triangles.size() < 16) {
    generatePackedTriangles();
    return;
  }

  generateChildren();

  std::vector<Triangle> remainingTriangles;
  for (auto triangle : triangles) {
    BoundingBox triangleAABB = triangle.getBoundingBox();

    bool foundSubnode = false;
    for (auto child : children) {
      BoundingBox temp(child->aabb.min, child->aabb.max);
      if (temp.fullyContains(triangleAABB)) {
        child->addTriangle(triangle);
        foundSubnode = true;
      }
    }

    if (!foundSubnode) {
      remainingTriangles.push_back(triangle);
    }
  }

  triangles.clear();
  remainingTriangles.swap(triangles);

  std::vector<Sphere> remainingSpheres;
  for (auto sphere : spheres) {
    BoundingBox triangleAABB = sphere.getBoundingBox();

    bool foundSubnode = false;
    for (auto child : children) {
      BoundingBox temp(child->aabb.min, child->aabb.max);
      if (temp.fullyContains(triangleAABB)) {
        child->addSphere(sphere);
        foundSubnode = true;
      }
    }

    if (!foundSubnode) {
      remainingSpheres.push_back(sphere);
    }
  }

  spheres.clear();
  remainingSpheres.swap(spheres);

  generatePackedTriangles();

  for (auto child : children) {
    child->subdivide();
  }
}

bool OctreeNode::hit(Ray ray, Primitive::HitDescriptor &hitDescriptor) const {
  Primitive::HitDescriptor nearestHit;
  float nearestDistance = std::numeric_limits<float>::max();

  PackedRay packedRay;
  for (int k = 0; k < 8; k++) {
    packedRay.direction[0].m256_f32[k] = ray.direction.x;
    packedRay.direction[1].m256_f32[k] = ray.direction.y;
    packedRay.direction[2].m256_f32[k] = ray.direction.z;

    packedRay.origin[0].m256_f32[k] = ray.origin.x;
    packedRay.origin[1].m256_f32[k] = ray.origin.y;
    packedRay.origin[2].m256_f32[k] = ray.origin.z;
  }

  for (auto pack : packedTriangles) {
    PackedIntersectionResult result;
    Primitive::HitDescriptor tempHit;
    float intersectionDistance;
    if (!pack.hit(packedRay, result, tempHit)) {
      continue;
    }

    intersectionDistance = ray.origin.distance(tempHit.position);
    if (nearestHit.primitive != nullptr &&
        intersectionDistance > nearestDistance) {
      continue;
    }

    nearestHit = tempHit;
    nearestDistance = intersectionDistance;
  }

  for (auto sphere : spheres) {
    Primitive::HitDescriptor tempHit;
    float intersectionDistance;
    if (!sphere.hit(ray, tempHit)) {
      continue;
    }

    intersectionDistance = ray.origin.distance(tempHit.position);
    if (nearestHit.primitive != nullptr &&
        intersectionDistance > nearestDistance) {
      continue;
    }

    nearestHit = tempHit;
    nearestDistance = intersectionDistance;
  }

  using node_distance = std::pair<OctreeNode*, float>;
  std::vector<node_distance> considered_children;
  considered_children.reserve(children.size());

  for (auto child : children) {
    float distance = std::numeric_limits<float>::max();
    BoundingBox childAABB(child->aabb.min, child->aabb.max);

    if (!childAABB.intersect(ray, distance)) {
      continue;
    }
    considered_children.emplace_back(child, distance);
  }

  std::sort(considered_children.begin(), considered_children.end(),
            [](const node_distance &a, const node_distance &b) {
              return a.second < b.second;
            });

  for (const auto [n_ptr, dist] : considered_children) {
    const OctreeNode &n = *n_ptr;

    Vector3f intersectionPoint;
    float interestionDistance;

    Primitive::HitDescriptor tempHit;
    if (!n.hit(ray, tempHit)) {
      continue;
    }

    interestionDistance = ray.origin.distance(tempHit.position);
    if (nearestHit.primitive != nullptr && interestionDistance > nearestDistance) {
      continue;
    }

    nearestHit = tempHit;
    nearestDistance = dist;
    break;
  }

  if (nearestHit.primitive == nullptr) {
    return false;
  }

  hitDescriptor = nearestHit;
  return true;
}