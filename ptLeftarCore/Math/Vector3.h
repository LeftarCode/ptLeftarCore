#pragma once
#include <math.h>
#include "Base.h"

class Vector3f { 
public:
  float x, y, z;

public:
  Vector3f() {}
  Vector3f(float x, float y, float z) : x(x), y(y), z(z) {
  
  }

  float length() { return sqrt(squareLength()); }

  float squareLength() const { return x * x + y * y + z * z; }

  void normalize() { 
    float vectorLength = this->length();
    x /= vectorLength;
    y /= vectorLength;
    z /= vectorLength;
  }

  float dotProduct(const Vector3f &v1) const { 
    return (x * v1.x + y * v1.y + z * v1.z);
  }

  float distance(const Vector3f &v1) const {
    const float dx = x - v1.x;
    const float dy = y - v1.y;
    const float dz = z - v1.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
  }

  Vector3f crossProduct(const Vector3f& v1) const {
    Vector3f result;
    result.x = y * v1.z - z * v1.y;
    result.y = -(x * v1.z - z * v1.x);
    result.z = x * v1.y - y * v1.x;

    return result;
  }

  Vector3f operator+(const Vector3f& v1) const {
    Vector3f result;
    result.x = x + v1.x;
    result.y = y + v1.y;
    result.z = z + v1.z;

    return result;
  }

  Vector3f operator-(const Vector3f& v1) const {
    Vector3f result;
    result.x = x - v1.x;
    result.y = y - v1.y;
    result.z = z - v1.z;

    return result;
  }

  Vector3f operator*(const Vector3f& v1) const {
    Vector3f result;
    result.x = x * v1.x;
    result.y = y * v1.y;
    result.z = z * v1.z;

    return result;
  }

  Vector3f operator/(const Vector3f& v1) const {
    Vector3f result;
    result.x = x / v1.x;
    result.y = y / v1.y;
    result.z = z / v1.z;

    return result;
  }

  Vector3f operator+(float value) const {
    Vector3f result;
    result.x = x + value;
    result.y = y + value;
    result.z = z + value;

    return result;
  }

  Vector3f operator-(float value) const {
    Vector3f result;
    result.x = x - value;
    result.y = y - value;
    result.z = z - value;

    return result;
  }

  Vector3f operator*(float value) const {
    Vector3f result;
    result.x = x * value;
    result.y = y * value;
    result.z = z * value;

    return result;
  }

  Vector3f operator/(float value) const {
    Vector3f result;
    result.x = x / value;
    result.y = y / value;
    result.z = z / value;

    return result;
  }

  static Vector3f random(float min, float max) {
    return Vector3f(randomFloat(min, max), randomFloat(min, max),
                    randomFloat(min, max));
  }

  static Vector3f randomUnitSphere() {
    while (true) {
      auto p = random(-1, 1);
      auto length = p.length();
      if (length * length >= 1)
        continue;
      return p;
    }
  }
};
