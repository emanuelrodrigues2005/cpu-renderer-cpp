#include "pipeline/DepthBuffer.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace cg {

DepthBuffer::DepthBuffer(int width, int height)
    : width_(width > 0 ? width : 0),
      height_(height > 0 ? height : 0),
      values_(static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_), 0.0F) {}

int DepthBuffer::width() const {
    return width_;
}

int DepthBuffer::height() const {
    return height_;
}

void DepthBuffer::clear() {
    std::fill(values_.begin(), values_.end(), 0.0F);
}

bool DepthBuffer::testAndSet(int x, int y, float invZ) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return false;
    }
    if (!std::isfinite(invZ)) {
        return false;
    }

    const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
                              static_cast<std::size_t>(x);

    if (invZ <= values_[index]) {
        return false;
    }

    values_[index] = invZ;
    return true;
}

float DepthBuffer::at(int x, int y) const {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return 0.0F;
    }

    const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
                              static_cast<std::size_t>(x);
    return values_[index];
}

}
