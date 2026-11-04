#pragma once

#include "canvas/Canvas.h"
#include "math/Vec3.h"
#include "pipeline/DepthBuffer.h"
#include "pipeline/Projection.h"
#include "shading/Lighting.h"

namespace cg {

struct ShadedVertex {
    ScreenPoint screen;
    float invZ = 0.0F;
    Vec3 world;
    Vec3 normal;
};

void fillTriangleShaded(Canvas& canvas, DepthBuffer& depth, const ShadedVertex& a,
                        const ShadedVertex& b, const ShadedVertex& c, const Vec3& eye,
                        const Lighting& lighting);

}
