#include "Vector3.h"

Vector3f::Vector3f() {}
Vector3f::Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}

float Vector3f::length() {
  return sqrtf(squareLength());
}

float Vector3f::squareLength() const {
  return x * x + y * y + z * z;
}

void Vector3f::normalize() {
  float vectorLength = this->length();
  x /= vectorLength;
  y /= vectorLength;
  z /= vectorLength;
}

float Vector3f::dotProduct(const Vector3f& v1) const {
  return (x * v1.x + y * v1.y + z * v1.z);
}

float Vector3f::distance(const Vector3f& v1) const {
  const float dx = x - v1.x;
  const float dy = y - v1.y;
  const float dz = z - v1.z;
  return sqrtf(dx * dx + dy * dy + dz * dz);
}

Vector3f Vector3f::crossProduct(const Vector3f& v1) const {
  Vector3f result;
  result.x = y * v1.z - z * v1.y;
  result.y = -(x * v1.z - z * v1.x);
  result.z = x * v1.y - y * v1.x;

  return result;
}

Vector3f Vector3f::operator+(const Vector3f& v1) const {
  Vector3f result;
  result.x = x + v1.x;
  result.y = y + v1.y;
  result.z = z + v1.z;

  return result;
}

Vector3f Vector3f::operator-(const Vector3f& v1) const {
  Vector3f result;
  result.x = x - v1.x;
  result.y = y - v1.y;
  result.z = z - v1.z;

  return result;
}

Vector3f Vector3f::operator*(const Vector3f& v1) const {
  Vector3f result;
  result.x = x * v1.x;
  result.y = y * v1.y;
  result.z = z * v1.z;

  return result;
}

Vector3f Vector3f::operator/(const Vector3f& v1) const {
  Vector3f result;
  result.x = x / v1.x;
  result.y = y / v1.y;
  result.z = z / v1.z;

  return result;
}

Vector3f Vector3f::operator+(float value) const {
  Vector3f result;
  result.x = x + value;
  result.y = y + value;
  result.z = z + value;

  return result;
}

Vector3f Vector3f::operator-(float value) const {
  Vector3f result;
  result.x = x - value;
  result.y = y - value;
  result.z = z - value;

  return result;
}

Vector3f Vector3f::operator*(float value) const {
  Vector3f result;
  result.x = x * value;
  result.y = y * value;
  result.z = z * value;

  return result;
}

Vector3f Vector3f::operator/(float value) const {
  Vector3f result;
  result.x = x / value;
  result.y = y / value;
  result.z = z / value;

  return result;
}

Vector3f Vector3f::random(float min, float max) {
  return Vector3f(randomFloat(min, max), randomFloat(min, max),
                  randomFloat(min, max));
}

Vector3f Vector3f::randomUnitSphere() {
  while (true) {
    auto p = random(-1, 1);
    auto length = p.length();
    if (length * length >= 1)
      continue;
    return p;
  }
}

Vector3f Vector3f::randomInHemisphere(const Vector3f& vec) {
  Vector3f unitSphere = randomUnitSphere();
  if (unitSphere.dotProduct(vec) > 0.0) {
    return unitSphere;
  } else {
    return Vector3f(unitSphere.x, unitSphere.y, unitSphere.z);
  }
}
