#include "Octree.h"
#include <limits>

Octree::Octree(std::vector<Triangle> scene) {

  float highestX = std::numeric_limits<float>::min(),
        lowestX = std::numeric_limits<float>::max();
  float highestY = std::numeric_limits<float>::min(),
        lowestY = std::numeric_limits<float>::max();
  float highestZ = std::numeric_limits<float>::min(),
        lowestZ = std::numeric_limits<float>::max();
  for (auto triangle : scene) {
    highestX = std::max(highestX, triangle.v1.position.x);
    highestX = std::max(highestX, triangle.v2.position.x);
    highestX = std::max(highestX, triangle.v3.position.x);
    highestY = std::max(highestY, triangle.v1.position.y);
    highestY = std::max(highestY, triangle.v2.position.y);
    highestY = std::max(highestY, triangle.v3.position.y);
    highestZ = std::max(highestZ, triangle.v1.position.z);
    highestZ = std::max(highestZ, triangle.v2.position.z);
    highestZ = std::max(highestZ, triangle.v3.position.z);

    lowestX = std::min(lowestX, triangle.v1.position.x);
    lowestX = std::min(lowestX, triangle.v2.position.x);
    lowestX = std::min(lowestX, triangle.v3.position.x);
    lowestY = std::min(lowestY, triangle.v1.position.y);
    lowestY = std::min(lowestY, triangle.v2.position.y);
    lowestY = std::min(lowestY, triangle.v3.position.y);
    lowestZ = std::min(lowestZ, triangle.v1.position.z);
    lowestZ = std::min(lowestZ, triangle.v2.position.z);
    lowestZ = std::min(lowestZ, triangle.v3.position.z);
  }

  topLeftFront = Vector3f(lowestX, lowestY, lowestZ);
  bottomRightBack = Vector3f(highestX, highestY, highestZ);
  
  rootNode = new OctreeNode(topLeftFront, bottomRightBack);
  for (auto triangle : scene) {
    rootNode->addTriangle(triangle);
  }
  rootNode->subdivide(depth);

}

bool Octree::hit(Ray ray, Vector3f& normal) { 
  return rootNode->hit(ray, normal);
}