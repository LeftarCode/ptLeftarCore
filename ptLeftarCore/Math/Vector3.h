#pragma once
#include <math.h>

template <class T>
class Vector3 { 
public:
  T x, y, z;

public:
  Vector3<T>() {}
  Vector3<T>(T x, T y, T z) : x(x), y(y), z(z) {}

  T length() { 
    return sqrt(x * x + y * y + z * z);
  }

  void normalize() { 
    T vectorLength = this->length();
    x /= vectorLength;
    y /= vectorLength;
    z /= vectorLength;
  }

  T dotProduct(const Vector3<T> &v1) const { 
    return (x * v1.x + y * v1.y + z * v1.z);
  }

  Vector3<T> crossProduct(const Vector3<T>& v1) const {
    Vector3<T> result;
    result.x = y * v1.z - z * v1.y;
    result.y = -(x * v1.z - z * v1.x);
    result.z = x * v1.y - y * v1.x;

    return result;
  }

  Vector3<T> operator+(const Vector3<T>& v1) const {
    Vector3<T> result;
    result.x = x + v1.x;
    result.y = y + v1.y;
    result.z = z + v1.z;

    return result;
  }

  Vector3<T> operator-(const Vector3<T>& v1) const {
    Vector3<T> result;
    result.x = x - v1.x;
    result.y = y - v1.y;
    result.z = z - v1.z;

    return result;
  }

  Vector3<T> operator*(const Vector3<T>& v1) const {
    Vector3<T> result;
    result.x = x * v1.x;
    result.y = y * v1.y;
    result.z = z * v1.z;

    return result;
  }

  Vector3<T> operator/(const Vector3<T>& v1) const {
    Vector3<T> result;
    result.x = x / v1.x;
    result.y = y / v1.y;
    result.z = z / v1.z;

    return result;
  }

  Vector3<T> operator+(T value) const {
    Vector3<T> result;
    result.x = x + value;
    result.y = y + value;
    result.z = z + value;

    return result;
  }

  Vector3<T> operator-(T value) const {
    Vector3<T> result;
    result.x = x - value;
    result.y = y - value;
    result.z = z - value;

    return result;
  }

  Vector3<T> operator*(T value) const {
    Vector3<T> result;
    result.x = x * value;
    result.y = y * value;
    result.z = z * value;

    return result;
  }

  Vector3<T> operator/(T value) const {
    Vector3<T> result;
    result.x = x / value;
    result.y = y / value;
    result.z = z / value;

    return result;
  }
};

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;