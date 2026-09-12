#pragma once

#include <string>

#include "math/Vec3.h"

namespace cg {

struct Camera {
    Vec3 u;
    Vec3 v;
    Vec3 n;
    Vec3 c;
    float d = 0.0F;
    float hx = 0.0F;
    float hy = 0.0F;

    Vec3 worldToView(const Vec3& point) const;
};

bool loadCameraFromFile(const std::string& path, Camera& camera, std::string& error);

}
