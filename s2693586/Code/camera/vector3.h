#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>    // For mathematical operations like sqrt, pow
#include <iostream> // For output stream operations

// Class representing a 3D vector with basic vector operations
class Vector3
{
public:
    float x, y, z; // Components of the vector

    // Constructors
    Vector3() : x(0), y(0), z(0) {}                          // Default constructor (zero vector)
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {} // Constructor with specific x, y, z values
    Vector3(float value) : x(value), y(value), z(value) {}   // Constructor with all components set to the same value

    // Clamps each component of the vector to the range [minValue, maxValue]
    Vector3 clamp(float minValue, float maxValue) const
    {
        return Vector3(
            std::max(minValue, std::min(x, maxValue)),
            std::max(minValue, std::min(y, maxValue)),
            std::max(minValue, std::min(z, maxValue)));
    }

    // Basic arithmetic operators
    Vector3 operator+(const Vector3 &v) const { return Vector3(x + v.x, y + v.y, z + v.z); }      // Vector addition
    Vector3 operator-(const Vector3 &v) const { return Vector3(x - v.x, y - v.y, z - v.z); }      // Vector subtraction
    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); } // Scalar multiplication
    Vector3 operator/(float scalar) const
    {
        if (this == nullptr)
        {
            std::cerr << "Error: Invalid 'this' pointer in Vector3::operator/" << std::endl;
            return Vector3(0, 0, 0); // or handle as needed
        }
        if (scalar == 0.0f)
        {
            std::cerr << "Warning: Division by zero in Vector3::operator/." << std::endl;
            return *this; // or handle zero division as appropriate
        }
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    // Element-wise multiplication
    Vector3 operator*(const Vector3 &v) const { return Vector3(x * v.x, y * v.y, z * v.z); }

    // Compound assignment operators
    Vector3 &operator+=(const Vector3 &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vector3 &operator-=(const Vector3 &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    Vector3 &operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    Vector3 &operator/=(float scalar)
    {
        if (scalar != 0)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }
        throw std::runtime_error("Division by zero in Vector3");
    }

    // Unary negation operator
    Vector3 operator-() const { return Vector3(-x, -y, -z); }

    // Vector operations
    float dot(const Vector3 &v) const { return x * v.x + y * v.y + z * v.z; }                                          // Dot product
    Vector3 cross(const Vector3 &v) const { return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); } // Cross product

    // Magnitude and normalization
    float length() const { return std::sqrt(x * x + y * y + z * z); } // Compute the vector's magnitude
    Vector3 normalize() const
    {
        float len = length();
        if (len > 0)
        {
            return *this / len; // Normalize by dividing each component by the length
        }
        return Vector3(0, 0, 0); // Return zero vector if the length is zero
    }

    // Static zero vector
    static Vector3 zero() { return Vector3(0, 0, 0); }

    // Comparison operators
    bool operator==(const Vector3 &v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vector3 &v) const { return !(*this == v); }

    // Friend functions for scalar multiplication and output
    friend Vector3 operator*(float scalar, const Vector3 &v) { return Vector3(v.x * scalar, v.y * scalar, v.z * scalar); } // Equality comparison
    friend std::ostream &operator<<(std::ostream &os, const Vector3 &v)                                                    // Inequality comparison
    {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
    float &operator[](int index)
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            throw std::out_of_range("Index out of range for Vector3");
        }
    }
    // Element access (by index)
    const float &operator[](int index) const
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            throw std::out_of_range("Index out of range for Vector3");
        }
    }
    // Element-wise division
    Vector3 operator/(const Vector3 &v) const
    {
        if (v.x == 0 || v.y == 0 || v.z == 0)
        {
            std::cerr << "Warning: Division by zero component in Vector3::operator/." << std::endl;
            return *this; // or handle as appropriate
        }
        return Vector3(x / v.x, y / v.y, z / v.z);
    }
};

#endif // VECTOR3_H
