#pragma once

#include "Camera.h"
#include "Vec3.h"

namespace cg {

struct ScreenPoint {
    int x = 0;
    int y = 0;
};

bool projectToScreen(const Vec3& viewPoint, const Camera& camera, int width, int height,
                     ScreenPoint& screen);

}
