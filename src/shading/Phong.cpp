#include "shading/Phong.h"

#include <cmath>

namespace cg {

namespace {

bool isFinite(const Vec3& vector) {
    return std::isfinite(vector.x) && std::isfinite(vector.y) && std::isfinite(vector.z);
}

bool isFinite(const Color& color) {
    return std::isfinite(color.r) && std::isfinite(color.g) && std::isfinite(color.b);
}

Color ambientColor(const Lighting& lighting) {
    return Color{lighting.light.ambient.r * lighting.material.ka,
                 lighting.light.ambient.g * lighting.material.ka,
                 lighting.light.ambient.b * lighting.material.ka}
        .clamped();
}

}

Color phongColor(const Vec3& point, const Vec3& normal, const Vec3& eye,
                 const Lighting& lighting) {
    const Color ambient = ambientColor(lighting);

    if (!isFinite(point) || !isFinite(normal) || !isFinite(eye) || normal.length() == 0.0F) {
        return ambient;
    }
    if (!isFinite(lighting.light.position)) {
        return ambient;
    }

    const Vec3 lightDirection = (lighting.light.position - point).normalized();
    const Vec3 viewDirection = (eye - point).normalized();
    if (!isFinite(lightDirection) || !isFinite(viewDirection) ||
        lightDirection.length() == 0.0F || viewDirection.length() == 0.0F) {
        return ambient;
    }

    Vec3 n = normal.normalized();
    if (!isFinite(n)) {
        return ambient;
    }

    float nDotL = n.dot(lightDirection);
    if (nDotL < 0.0F && n.dot(viewDirection) < 0.0F) {
        n = -n;
        nDotL = n.dot(lightDirection);
    }

    Color result = ambient;
    if (nDotL > 0.0F) {
        if (isFinite(lighting.light.color) && isFinite(lighting.material.kd) &&
            isFinite(lighting.material.od)) {
            const Vec3 kdOd = lighting.material.kd.componentMultiply(lighting.material.od);
            result = result + Color{lighting.light.color.r * nDotL * kdOd.x,
                                    lighting.light.color.g * nDotL * kdOd.y,
                                    lighting.light.color.b * nDotL * kdOd.z};
        }

        const Vec3 reflection = (n * (2.0F * nDotL) - lightDirection).normalized();
        const float rDotV = reflection.dot(viewDirection);
        if (rDotV > 0.0F && isFinite(lighting.light.color) &&
            std::isfinite(lighting.material.ks) && std::isfinite(lighting.material.eta)) {
            const float specularFactor =
                std::pow(rDotV, lighting.material.eta) * lighting.material.ks;
            result = result + Color{lighting.light.color.r * specularFactor,
                                    lighting.light.color.g * specularFactor,
                                    lighting.light.color.b * specularFactor};
        }
    }

    return result.clamped();
}

}
