#pragma once

#include "pipeline/Camera.h"
#include "math/Vec3.h"

namespace cg {

struct NormalizedPoint {
    float x = 0.0F;
    float y = 0.0F;
};

struct ScreenPoint {
    int x = 0;
    int y = 0;
};

struct ProjectedVertex {
    ScreenPoint screen;
    float viewZ = 0.0F;
    float invZ = 0.0F;
};

bool projectToNormalized(const Vec3& viewPoint, const Camera& camera,
                         NormalizedPoint& normalized);

void normalizedToScreen(const NormalizedPoint& normalized, int width, int height,
                        ScreenPoint& screen);

bool projectToScreen(const Vec3& viewPoint, const Camera& camera, int width, int height,
                     ScreenPoint& screen);

bool projectToScreenWithDepth(const Vec3& viewPoint, const Camera& camera, int width, int height,
                              ProjectedVertex& projected);

}
