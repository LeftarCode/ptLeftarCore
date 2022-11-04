#pragma once
#include <math.h>
#include "Base.h"

class Vector3f { 
public:
  float x, y, z;

public:
  Vector3f();
  Vector3f(float x, float y, float z);

  float length();

  float squareLength() const;

  void normalize();

  float dotProduct(const Vector3f &v1) const;

  float distance(const Vector3f &v1) const;

  Vector3f crossProduct(const Vector3f &v1) const;

  Vector3f operator+(const Vector3f &v1) const;

  Vector3f operator-(const Vector3f &v1) const;

  Vector3f operator*(const Vector3f &v1) const;

  Vector3f operator/(const Vector3f &v1) const;

  Vector3f operator+(float value) const;

  Vector3f operator-(float value) const;

  Vector3f operator*(float value) const;

  Vector3f operator/(float value) const;

  static Vector3f random(float min, float max);

  static Vector3f randomUnitSphere();

  static Vector3f randomInHemisphere(const Vector3f &vec);
};
