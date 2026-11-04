#pragma once

#include <string>

#include "math/Color.h"
#include "math/Vec3.h"

namespace cg {

struct Light {
    Color ambient;
    Color color;
    Vec3 position;
};

struct Material {
    float ka = 0.0F;
    Vec3 kd;
    Vec3 od;
    float ks = 0.0F;
    float eta = 1.0F;
};

struct Lighting {
    Light light;
    Material material;
};

bool loadLightingFromFile(const std::string& path, Lighting& lighting, std::string& error);

}
