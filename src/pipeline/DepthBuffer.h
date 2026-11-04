#pragma once

#include <vector>

namespace cg {

class DepthBuffer {
public:
    DepthBuffer(int width, int height);

    int width() const;
    int height() const;

    void clear();

    bool testAndSet(int x, int y, float invZ);
    float at(int x, int y) const;

private:
    int width_ = 0;
    int height_ = 0;
    std::vector<float> values_;
};

}
