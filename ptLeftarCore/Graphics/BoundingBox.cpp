#include <algorithm>
#include "BoundingBox.h"
#include <utility>

BoundingBox::BoundingBox(Vector3f min, Vector3f max)
    : min(min), max(max) {

  float midx = (min.x + max.x) / 2;
  float midy = (min.y + max.y) / 2;
  float midz = (min.z + max.z) / 2;
  center = Vector3f(midx, midy, midz);
}

BoundingBox::BoundingBox() {
  min = Vector3f(std::numeric_limits<float>::max(),
                 std::numeric_limits<float>::max(),
                 std::numeric_limits<float>::max());
  max = Vector3f(std::numeric_limits<float>::min(),
                 std::numeric_limits<float>::min(),
                 std::numeric_limits<float>::min());
}

void BoundingBox::extend(Vector3f point) {
  min.x = std::min(min.x, point.x);
  min.y = std::min(min.y, point.y);
  min.z = std::min(min.z, point.z);

  max.x = std::max(max.x, point.x);
  max.y = std::max(max.y, point.y);
  max.z = std::max(max.z, point.z);

  float midx = (min.x + max.x) / 2;
  float midy = (min.y + max.y) / 2;
  float midz = (min.z + max.z) / 2;

  center = Vector3f(midx, midy, midz);
}

bool BoundingBox::contains(Vector3f point) {
  return point.x >= min.x && point.x <= max.x &&
         point.y >= min.y && point.y <= max.y &&
         point.z >= min.z && point.z <= max.z;
}

bool BoundingBox::fullyContains(BoundingBox aabb) {
  return contains(aabb.min) && contains(aabb.max);
}

bool BoundingBox::intersect(Ray ray, float& distance) {
  const Vector3f &dirfrac = ray.invDirection;

  float t1 = (min.x - ray.origin.x) * dirfrac.x;
  float t2 = (max.x - ray.origin.x) * dirfrac.x;
  float t3 = (min.y - ray.origin.y) * dirfrac.y;
  float t4 = (max.y - ray.origin.y) * dirfrac.y;
  float t5 = (min.z - ray.origin.z) * dirfrac.z;
  float t6 = (max.z - ray.origin.z) * dirfrac.z;

  float tmax =
      std::min({std::max(t1, t2), std::max(t3, t4), std::max(t5, t6)});
  if (tmax < 0.0) {
    return false;
  }

  float tmin =
      std::max({std::min(t1, t2), std::min(t3, t4), std::min(t5, t6)});
  if (tmin > tmax) {
    return false;
  }

  distance = tmin;
  return true;
}