#include "Vec3.h"

#include <cmath>

namespace cg {

Vec3 Vec3::operator+(const Vec3& other) const {
    return Vec3{x + other.x, y + other.y, z + other.z};
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return Vec3{x - other.x, y - other.y, z - other.z};
}

float Vec3::dot(const Vec3& other) const { return x * other.x + y * other.y + z * other.z; }

Vec3 Vec3::cross(const Vec3& other) const {
    return Vec3{y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
}

float Vec3::length() const { return std::sqrt(dot(*this)); }

Vec3 Vec3::normalized() const {
    const float len = length();
    if (len == 0.0F) {
        return Vec3{};
    }
    return Vec3{x / len, y / len, z / len};
}

}
