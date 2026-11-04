#pragma once

#include <cstdint>

namespace cg {

struct Color {
    float r = 0.0F;
    float g = 0.0F;
    float b = 0.0F;

    Color operator+(const Color& other) const;
    Color operator*(float scalar) const;

    Color clamped() const;
    std::uint32_t packed() const;
};

Color operator*(float scalar, const Color& color);

}
