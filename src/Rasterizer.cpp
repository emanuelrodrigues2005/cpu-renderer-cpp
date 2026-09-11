#include "Rasterizer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace cg {

void drawLine(Canvas& canvas, const ScreenPoint& a, const ScreenPoint& b, std::uint32_t color) {
    int x = a.x;
    int y = a.y;
    const int deltaX = std::abs(b.x - a.x);
    const int deltaY = -std::abs(b.y - a.y);
    const int stepX = a.x < b.x ? 1 : -1;
    const int stepY = a.y < b.y ? 1 : -1;
    int error = deltaX + deltaY;

    while (true) {
        canvas.drawPixel(x, y, color);
        if (x == b.x && y == b.y) {
            break;
        }
        const int doubledError = 2 * error;
        if (doubledError >= deltaY) {
            error += deltaY;
            x += stepX;
        }
        if (doubledError <= deltaX) {
            error += deltaX;
            y += stepY;
        }
    }
}

void fillTriangle(Canvas& canvas, const ScreenPoint& a, const ScreenPoint& b, const ScreenPoint& c,
                  std::uint32_t color) {
    const long long area = static_cast<long long>(b.x - a.x) * (c.y - a.y) -
                           static_cast<long long>(b.y - a.y) * (c.x - a.x);
    if (area == 0) {
        return;
    }

    const ScreenPoint* vertices[3] = {&a, &b, &c};
    std::sort(vertices, vertices + 3, [](const ScreenPoint* first, const ScreenPoint* second) {
        return first->y < second->y;
    });

    const ScreenPoint* edges[3][2] = {{vertices[0], vertices[1]},
                                      {vertices[1], vertices[2]},
                                      {vertices[0], vertices[2]}};

    for (int y = std::max(vertices[0]->y, 0); y <= std::min(vertices[2]->y, canvas.height() - 1);
         ++y) {
        float minimum = 0.0F;
        float maximum = 0.0F;
        int intersections = 0;

        for (const auto& edge : edges) {
            const ScreenPoint& first = *edge[0];
            const ScreenPoint& second = *edge[1];
            if (first.y == second.y) {
                continue;
            }
            const int low = std::min(first.y, second.y);
            const int high = std::max(first.y, second.y);
            if (y < low || y > high) {
                continue;
            }

            const float t =
                static_cast<float>(y - first.y) / static_cast<float>(second.y - first.y);
            const float x =
                static_cast<float>(first.x) + t * static_cast<float>(second.x - first.x);
            if (intersections == 0 || x < minimum) {
                minimum = x;
            }
            if (intersections == 0 || x > maximum) {
                maximum = x;
            }
            ++intersections;
        }

        if (intersections < 2) {
            continue;
        }

        const int left = std::max(static_cast<int>(std::lround(minimum)), 0);
        const int right = std::min(static_cast<int>(std::lround(maximum)), canvas.width() - 1);
        for (int x = left; x <= right; ++x) {
            canvas.drawPixel(x, y, color);
        }
    }
}

}
