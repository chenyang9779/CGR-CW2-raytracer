#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <iostream>

class Vector3 {
public:
    float x, y, z;

    // Constructors
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Basic arithmetic operators
    Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    Vector3 operator/(float scalar) const {
        if (scalar != 0) {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }
        throw std::runtime_error("Division by zero in Vector3");
    }

    // Element-wise multiplication
    Vector3 operator*(const Vector3& v) const { return Vector3(x * v.x, y * v.y, z * v.z); }

    // Compound assignment operators
    Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vector3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vector3& operator/=(float scalar) {
        if (scalar != 0) {
            x /= scalar; y /= scalar; z /= scalar;
            return *this;
        }
        throw std::runtime_error("Division by zero in Vector3");
    }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }


    // Vector operations
    float dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vector3 cross(const Vector3& v) const { return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }

    // Magnitude and normalization
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vector3 normalize() const {
        float len = length();
        if (len > 0) {
            return *this / len;
        }
        return Vector3(0, 0, 0); // Return zero vector if length is zero
    }

    // Static zero vector
    static Vector3 zero() { return Vector3(0, 0, 0); }

    // Comparison operators
    bool operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vector3& v) const { return !(*this == v); }

    // Friend functions
    friend Vector3 operator*(float scalar, const Vector3& v) { return Vector3(v.x * scalar, v.y * scalar, v.z * scalar); }
    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};

#endif // VECTOR3_H
