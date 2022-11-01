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
    : topLeftFront(Vector3f()), bottomRightBack(Vector3f()) {}

OctreeNode::OctreeNode(Vector3f topLeftFront, Vector3f bottomRightBack)
    : topLeftFront(topLeftFront), bottomRightBack(bottomRightBack) {
}

void OctreeNode::addTriangle(Triangle triangle) {
  triangles.push_back(triangle);
}

void OctreeNode::subdivide() { 
  if (triangles.size() < 16) {
    return;
  }

  float midx = (topLeftFront.x + bottomRightBack.x) / 2;
  float midy = (topLeftFront.y + bottomRightBack.y) / 2;
  float midz = (topLeftFront.z + bottomRightBack.z) / 2;

  for (int i = 0; i < 8; i++) {
    OctreeNode newNode = OctreeNode(topLeftFront, bottomRightBack);
    if (i == TopLeftFront) {
      Vector3f newTopLeftFront(topLeftFront.x, topLeftFront.y, topLeftFront.z);
      Vector3f newBottomRightBack(midx, midy, midz);
      newNode = OctreeNode(newTopLeftFront, newBottomRightBack);
    } else if (i == TopRightFront) {
      Vector3f newTopLeftFront(midx, topLeftFront.y, topLeftFront.z);
      Vector3f newBottomRightBack(bottomRightBack.x, midy, midz);
      newNode = OctreeNode(newTopLeftFront, newBottomRightBack);
    } else if (i == BottomRightFront) {
      Vector3f newTopLeftFront(topLeftFront.x, topLeftFront.y, midz);
      Vector3f newBottomRightBack(midx, midy, bottomRightBack.z);
      newNode = OctreeNode(newTopLeftFront, newBottomRightBack);
    } else if (i == BottomLeftFront) {
      Vector3f newTopLeftFront(midx, topLeftFront.y, midz);
      Vector3f newBottomRightBack(bottomRightBack.x, midy, bottomRightBack.z);
      newNode = OctreeNode(newTopLeftFront, newBottomRightBack);
    } else if (i == TopLeftBottom) {
      Vector3f newTopLeftFront(topLeftFront.x, midy, topLeftFront.z);
      Vector3f newBottomRightBack(midx, bottomRightBack.y, midz);
      newNode = OctreeNode(newTopLeftFront, newBottomRightBack);
    } else if (i == TopRightBottom) {
      Vector3f newTopLeftFront(midx, midy, topLeftFront.z);
      Vector3f newBottomRightBack(bottomRightBack.x, bottomRightBack.y, midz);
      newNode = OctreeNode(newTopLeftFront, newBottomRightBack);
    } else if (i == BottomRightBack) {
      Vector3f newTopLeftFront(topLeftFront.x, midy, midz);
      Vector3f newBottomRightBack(midx, bottomRightBack.y, bottomRightBack.z);
      newNode = OctreeNode(newTopLeftFront, newBottomRightBack);
    }  else if (i == BottomLeftBack) {
      Vector3f newTopLeftFront(midx, midy, midz);
      Vector3f newBottomRightBack(bottomRightBack.x, bottomRightBack.y,
                                  bottomRightBack.z);
      newNode = OctreeNode(newTopLeftFront, newBottomRightBack);
    }

    children.push_back(new OctreeNode(newNode.topLeftFront, newNode.bottomRightBack));
  }

  std::vector<Triangle> remainingTriangles;
  for (auto triangle : triangles) {
    AABB triangleAABB = triangle.getBoundingBox();

    bool foundSubnode = false;
    for (auto child : children) {
      AABB temp(child->topLeftFront, child->bottomRightBack);
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

  for (auto child : children) {
    child->subdivide();
  }
}

bool OctreeNode::hit(Ray ray, Primitive::HitDescriptor &hitDescriptor) const {
  //for (auto triangle : triangles) {
  //  if (triangle.hit(ray, hitDescriptor)) {
  //    return true;
  //  }
  //}

  //for (auto child : children) {
  //  AABB childAABB(child->topLeftFront, child->bottomRightBack);
  //  float distance = 0;
  //  if (childAABB.intersect(ray, distance)) {
  //    if (child->hit(ray, hitDescriptor)) {
  //      return true;
  //    }
  //  }
  //}

  //return false;
  Primitive::HitDescriptor nearestHit;
  float nearestDistance = std::numeric_limits<float>::max();

  for (auto triangle : triangles) {
    Primitive::HitDescriptor tempHit;
    float interestionDistance;
    if (!triangle.hit(ray, tempHit)) {
      continue;
    }

    interestionDistance = ray.origin.distance(tempHit.position);
    if (nearestHit.primitive != nullptr &&
        interestionDistance > nearestDistance) {
      continue;
    }

    nearestHit = tempHit;
    nearestDistance = interestionDistance;
  }

  using node_distance = std::pair<OctreeNode*, float>;
  std::vector<node_distance> considered_children;
  considered_children.reserve(children.size());

  for (auto child : children) {
    float distance = std::numeric_limits<float>::max();
    AABB childAABB(child->topLeftFront, child->bottomRightBack);

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