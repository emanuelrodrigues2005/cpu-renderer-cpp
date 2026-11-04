#pragma once

#include "math/Color.h"
#include "math/Vec3.h"
#include "shading/Lighting.h"

namespace cg {

Color phongColor(const Vec3& point, const Vec3& normal, const Vec3& eye,
                 const Lighting& lighting);

}
