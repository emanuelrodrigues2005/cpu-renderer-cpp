#include "pipeline/ShadedRasterizer.h"

#include <algorithm>
#include <cmath>

#include "shading/Phong.h"

namespace cg {

namespace {

long long edgeFunction(const ScreenPoint& a, const ScreenPoint& b, const ScreenPoint& p) {
    return (static_cast<long long>(b.x) - static_cast<long long>(a.x)) *
               (static_cast<long long>(p.y) - static_cast<long long>(a.y)) -
           (static_cast<long long>(b.y) - static_cast<long long>(a.y)) *
               (static_cast<long long>(p.x) - static_cast<long long>(a.x));
}

bool isFinite(const Vec3& vector) {
    return std::isfinite(vector.x) && std::isfinite(vector.y) && std::isfinite(vector.z);
}

Vec3 normalizedOr(const Vec3& vector, const Vec3& fallback) {
    const Vec3 normalized = vector.normalized();
    if (!isFinite(normalized) || normalized.length() == 0.0F) {
        return fallback;
    }
    return normalized;
}

}

void fillTriangleShaded(Canvas& canvas, DepthBuffer& depth, const ShadedVertex& a,
                        const ShadedVertex& b, const ShadedVertex& c, const Vec3& eye,
                        const Lighting& lighting) {
    if (!std::isfinite(a.invZ) || !std::isfinite(b.invZ) || !std::isfinite(c.invZ) ||
        a.invZ <= 0.0F || b.invZ <= 0.0F || c.invZ <= 0.0F) {
        return;
    }

    const long long area = (static_cast<long long>(b.screen.x) -
                            static_cast<long long>(a.screen.x)) *
                               (static_cast<long long>(c.screen.y) -
                                static_cast<long long>(a.screen.y)) -
                           (static_cast<long long>(b.screen.y) -
                            static_cast<long long>(a.screen.y)) *
                               (static_cast<long long>(c.screen.x) -
                                static_cast<long long>(a.screen.x));
    if (area == 0) {
        return;
    }

    const float floatArea = static_cast<float>(area);
    if (!std::isfinite(floatArea) || floatArea == 0.0F) {
        return;
    }

    const Vec3 faceNormal = normalizedOr((b.world - a.world).cross(c.world - a.world), Vec3{});

    const ShadedVertex* vertices[3] = {&a, &b, &c};
    std::sort(vertices, vertices + 3,
              [](const ShadedVertex* first, const ShadedVertex* second) {
                  return first->screen.y < second->screen.y;
              });

    const ShadedVertex* edges[3][2] = {{vertices[0], vertices[1]},
                                       {vertices[1], vertices[2]},
                                       {vertices[0], vertices[2]}};

    for (int y = std::max(vertices[0]->screen.y, 0);
         y <= std::min(vertices[2]->screen.y, canvas.height() - 1); ++y) {
        float minimum = 0.0F;
        float maximum = 0.0F;
        int intersections = 0;

        for (const auto& edge : edges) {
            const ShadedVertex& first = *edge[0];
            const ShadedVertex& second = *edge[1];
            if (first.screen.y == second.screen.y) {
                continue;
            }
            const int low = std::min(first.screen.y, second.screen.y);
            const int high = std::max(first.screen.y, second.screen.y);
            if (y < low || y > high) {
                continue;
            }

            const float t =
                static_cast<float>(static_cast<long long>(y) -
                                   static_cast<long long>(first.screen.y)) /
                static_cast<float>(static_cast<long long>(second.screen.y) -
                                   static_cast<long long>(first.screen.y));
            const float x =
                static_cast<float>(first.screen.x) +
                t * static_cast<float>(static_cast<long long>(second.screen.x) -
                                       static_cast<long long>(first.screen.x));
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
            const ScreenPoint point{x, y};
            float wa = static_cast<float>(edgeFunction(b.screen, c.screen, point)) / floatArea;
            float wb = static_cast<float>(edgeFunction(c.screen, a.screen, point)) / floatArea;
            float wc = 1.0F - wa - wb;

            wa = std::max(wa, 0.0F);
            wb = std::max(wb, 0.0F);
            wc = std::max(wc, 0.0F);
            const float weightSum = wa + wb + wc;
            if (!std::isfinite(weightSum) || weightSum == 0.0F) {
                continue;
            }
            wa /= weightSum;
            wb /= weightSum;
            wc /= weightSum;

            const float invZ = wa * a.invZ + wb * b.invZ + wc * c.invZ;
            if (!std::isfinite(invZ) || invZ <= 0.0F) {
                continue;
            }
            if (!depth.testAndSet(x, y, invZ)) {
                continue;
            }

            const Vec3 world = (a.world * (wa * a.invZ) + b.world * (wb * b.invZ) +
                                c.world * (wc * c.invZ)) /
                               invZ;
            const Vec3 normal = normalizedOr((a.normal * (wa * a.invZ) +
                                              b.normal * (wb * b.invZ) +
                                              c.normal * (wc * c.invZ)) /
                                                 invZ,
                                             faceNormal);

            const Color color = phongColor(world, normal, eye, lighting);
            canvas.drawPixel(x, y, color.packed());
        }
    }
}

}
