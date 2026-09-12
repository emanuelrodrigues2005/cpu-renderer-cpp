#include "pipeline/Projection.h"

#include <cmath>

namespace cg {

bool projectToNormalized(const Vec3& viewPoint, const Camera& camera,
                         NormalizedPoint& normalized) {
    if (!std::isfinite(viewPoint.x) || !std::isfinite(viewPoint.y) ||
        !std::isfinite(viewPoint.z) || viewPoint.z <= 0.0F ||
        !std::isfinite(camera.d) || !std::isfinite(camera.hx) ||
        !std::isfinite(camera.hy) || camera.d <= 0.0F || camera.hx <= 0.0F ||
        camera.hy <= 0.0F) {
        return false;
    }

    normalized.x = (camera.d * viewPoint.x / viewPoint.z) / camera.hx;
    normalized.y = (camera.d * viewPoint.y / viewPoint.z) / camera.hy;
    return std::isfinite(normalized.x) && std::isfinite(normalized.y);
}

void normalizedToScreen(const NormalizedPoint& normalized, int width, int height,
                        ScreenPoint& screen) {
    screen.x = static_cast<int>(
        std::lround((normalized.x + 1.0F) * static_cast<float>(width) / 2.0F));
    screen.y = static_cast<int>(
        std::lround((1.0F - normalized.y) * static_cast<float>(height) / 2.0F));
}

bool projectToScreen(const Vec3& viewPoint, const Camera& camera, int width, int height,
                     ScreenPoint& screen) {
    NormalizedPoint normalized;
    if (!projectToNormalized(viewPoint, camera, normalized)) {
        return false;
    }

    normalizedToScreen(normalized, width, height, screen);
    return true;
}

}
