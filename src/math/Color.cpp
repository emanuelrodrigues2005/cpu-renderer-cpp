#include "math/Color.h"

#include <algorithm>
#include <cmath>

namespace cg {

namespace {

std::uint32_t packChannel(float value) {
    if (std::isnan(value)) {
        return 0U;
    }
    const float clamped = std::clamp(value, 0.0F, 255.0F);
    return static_cast<std::uint32_t>(std::lround(clamped));
}

}

Color Color::operator+(const Color& other) const {
    return Color{r + other.r, g + other.g, b + other.b};
}

Color Color::operator*(float scalar) const {
    return Color{r * scalar, g * scalar, b * scalar};
}

Color Color::clamped() const {
    return Color{std::clamp(r, 0.0F, 255.0F), std::clamp(g, 0.0F, 255.0F),
                 std::clamp(b, 0.0F, 255.0F)};
}

std::uint32_t Color::packed() const {
    return (packChannel(r) << 16) | (packChannel(g) << 8) | packChannel(b);
}

Color operator*(float scalar, const Color& color) { return color * scalar; }

}
