#pragma once

namespace cg {

struct Vec3 {
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;

    float dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
    float length() const;

    Vec3 normalized() const;
};

}
