#pragma once

#include <cstdint>

namespace cg {

constexpr std::uint32_t kBlack = 0x000000;
constexpr std::uint32_t kWhite = 0xFFFFFF;

class Canvas {
public:
    virtual ~Canvas() = default;

    virtual int width() const = 0;
    virtual int height() const = 0;

    virtual void clear(std::uint32_t color) = 0;
    virtual void drawPixel(int x, int y, std::uint32_t color) = 0;
};

}  // namespace cg
