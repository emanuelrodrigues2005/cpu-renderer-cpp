#pragma once

#include <cstdint>
#include <vector>

#include "Canvas.h"

namespace cg {

class MemoryCanvas final : public Canvas {
public:
    MemoryCanvas(int width, int height);

    int width() const override;
    int height() const override;

    void clear(std::uint32_t color) override;
    void drawPixel(int x, int y, std::uint32_t color) override;

    std::uint32_t pixelAt(int x, int y) const;

private:
    int width_ = 0;
    int height_ = 0;
    std::vector<std::uint32_t> pixels_;
};

}
