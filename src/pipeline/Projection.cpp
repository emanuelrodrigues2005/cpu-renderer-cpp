#include "pipeline/Projection.h"

#include <cmath>

namespace cg {

bool projectToScreen(const Vec3& viewPoint, const Camera& camera, int width, int height,
                     ScreenPoint& screen) {
    if (viewPoint.z <= 0.0F) {
        return false;
    }

    const float normalizedX = (camera.d * viewPoint.x / viewPoint.z) / camera.hx;
    const float normalizedY = (camera.d * viewPoint.y / viewPoint.z) / camera.hy;

    screen.x = static_cast<int>(
        std::lround((normalizedX + 1.0F) * static_cast<float>(width) / 2.0F));
    screen.y = static_cast<int>(
        std::lround((1.0F - normalizedY) * static_cast<float>(height) / 2.0F));
    return true;
}

}
