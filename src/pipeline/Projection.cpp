#include "pipeline/Projection.h"

#include <cmath>

namespace cg {

namespace {

constexpr float kMaxNormalizedCoordinate = 100000.0F;

}

bool projectToNormalized(const Vec3& viewPoint, const Camera& camera,
                         NormalizedPoint& normalized) {
    if (!std::isfinite(viewPoint.x) || !std::isfinite(viewPoint.y) ||
        !std::isfinite(viewPoint.z) || viewPoint.z <= 0.0F ||
        !std::isfinite(camera.d) || !std::isfinite(camera.hx) ||
        !std::isfinite(camera.hy) || camera.d <= 0.0F || camera.hx <= 0.0F ||
        camera.hy <= 0.0F) {
        return false;
    }

    const float nx = (camera.d * viewPoint.x / viewPoint.z) / camera.hx;
    const float ny = (camera.d * viewPoint.y / viewPoint.z) / camera.hy;

    if (!std::isfinite(nx) || !std::isfinite(ny) || std::fabs(nx) > kMaxNormalizedCoordinate ||
        std::fabs(ny) > kMaxNormalizedCoordinate) {
        return false;
    }

    normalized.x = nx;
    normalized.y = ny;
    return true;
}

void normalizedToScreen(const NormalizedPoint& normalized, int width, int height,
                        ScreenPoint& screen) {
    screen.x = static_cast<int>(
        std::lround((normalized.x + 1.0F) * static_cast<float>(width) / 2.0F));
    screen.y = static_cast<int>(
        std::lround((1.0F - normalized.y) * static_cast<float>(height) / 2.0F));
}

bool projectToScreenWithDepth(const Vec3& viewPoint, const Camera& camera, int width, int height,
                              ProjectedVertex& projected) {
    NormalizedPoint normalized;
    if (!projectToNormalized(viewPoint, camera, normalized)) {
        return false;
    }

    ProjectedVertex local;
    normalizedToScreen(normalized, width, height, local.screen);
    local.viewZ = viewPoint.z;
    local.invZ = 1.0F / local.viewZ;

    if (!std::isfinite(local.invZ)) {
        return false;
    }

    projected = local;
    return true;
}

bool projectToScreen(const Vec3& viewPoint, const Camera& camera, int width, int height,
                     ScreenPoint& screen) {
    ProjectedVertex projected;
    if (!projectToScreenWithDepth(viewPoint, camera, width, height, projected)) {
        return false;
    }

    screen = projected.screen;
    return true;
}

}
