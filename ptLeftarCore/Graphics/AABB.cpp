#include <algorithm>
#include "AABB.h"
#include <utility>

AABB::AABB(Vector3f topLeftFront, Vector3f bottomRightBack)
    : topLeftFront(topLeftFront), bottomRightBack(bottomRightBack) {

}

void AABB::extend(Vector3f point) {
  topLeftFront.x = std::min(topLeftFront.x, point.x);
  topLeftFront.y = std::min(topLeftFront.y, point.y);
  topLeftFront.z = std::min(topLeftFront.z, point.z);

  bottomRightBack.x = std::max(bottomRightBack.x, point.x);
  bottomRightBack.y = std::max(bottomRightBack.y, point.y);
  bottomRightBack.z = std::max(bottomRightBack.z, point.z);
}

bool AABB::contains(Vector3f point) {
  return point.x >= topLeftFront.x && point.x <= bottomRightBack.x &&
         point.y >= topLeftFront.y && point.y <= bottomRightBack.y &&
         point.z >= topLeftFront.z && point.z <= bottomRightBack.z;
}

bool AABB::fullyContains(AABB aabb) {
  return contains(aabb.topLeftFront) && contains(aabb.bottomRightBack);
}

bool AABB::intersect(Ray ray) {
  float tmin = (topLeftFront.x - ray.origin.x) / ray.direction.x;
  float tmax = (bottomRightBack.x - ray.origin.x) / ray.direction.x;

  if (tmin > tmax) {
    std::swap(tmin, tmax);
  }

  float tymin = (topLeftFront.y - ray.origin.y) / ray.direction.y;
  float tymax = (bottomRightBack.y - ray.origin.y) / ray.direction.y;

  if (tymin > tymax) {
    std::swap(tymin, tymax);
  }

  if ((tmin > tymax) || (tymin > tmax))
    return false;

  if (tymin > tmin)
    tmin = tymin;

  if (tymax < tmax)
    tmax = tymax;

  float tzmin = (topLeftFront.z - ray.origin.z) / ray.direction.z;
  float tzmax = (bottomRightBack.z - ray.origin.z) / ray.direction.z;

  if (tzmin > tzmax) {
    std::swap(tzmin, tzmax);
  }

  if ((tmin > tzmax) || (tzmin > tmax))
    return false;

  if (tzmin > tmin)
    tmin = tzmin;

  if (tzmax < tmax)
    tmax = tzmax;

  return true;
  /*float tx1 = (topLeftFront.x - ray.origin.x) * ray.invDirection.x;
  float tx2 = (bottomRightBack.x - ray.origin.x) * ray.invDirection.x;

  float tmin = std::min(tx1, tx2);
  float tmax = std::max(tx1, tx2);

  float ty1 = (topLeftFront.y - ray.origin.y) * ray.invDirection.y;
  float ty2 = (bottomRightBack.y - ray.origin.y) * ray.invDirection.y;

  tmin = std::max(tmin, std::min(ty1, ty2));
  tmax = std::min(tmax, std::max(ty1, ty2));

  return tmax >= tmin;*/
}