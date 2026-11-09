#include "math/Color.h"

#include <algorithm>
#include <cmath>

namespace cg {

namespace {

float clampChannel(float value) {
    if (std::isnan(value)) {
        return 0.0F;
    }
    return std::clamp(value, 0.0F, 255.0F);
}

std::uint32_t packChannel(float value) {
    return static_cast<std::uint32_t>(std::lround(clampChannel(value)));
}

}

Color Color::operator+(const Color& other) const {
    return Color{r + other.r, g + other.g, b + other.b};
}

Color Color::operator*(float scalar) const {
    return Color{r * scalar, g * scalar, b * scalar};
}

Color Color::clamped() const {
    return Color{clampChannel(r), clampChannel(g), clampChannel(b)};
}

std::uint32_t Color::packed() const {
    return (packChannel(r) << 16) | (packChannel(g) << 8) | packChannel(b);
}

Color operator*(float scalar, const Color& color) { return color * scalar; }

}
