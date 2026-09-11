#include "canvas/MemoryCanvas.h"

#include <algorithm>
#include <cstddef>

namespace cg {

MemoryCanvas::MemoryCanvas(int width, int height)
    : width_(width),
      height_(height),
      pixels_(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), kBlack) {}

int MemoryCanvas::width() const { return width_; }

int MemoryCanvas::height() const { return height_; }

void MemoryCanvas::clear(std::uint32_t color) {
    std::fill(pixels_.begin(), pixels_.end(), color);
}

void MemoryCanvas::drawPixel(int x, int y, std::uint32_t color) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return;
    }
    const std::size_t index =
        static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(x);
    pixels_[index] = color;
}

std::uint32_t MemoryCanvas::pixelAt(int x, int y) const {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return kBlack;
    }
    const std::size_t index =
        static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(x);
    return pixels_[index];
}

}
