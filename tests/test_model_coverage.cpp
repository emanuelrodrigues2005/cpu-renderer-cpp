#include <cstddef>
#include <cstdint>
#include <iostream>
#include <set>
#include <string>

#include "canvas/MemoryCanvas.h"
#include "mesh/Mesh.h"
#include "pipeline/Camera.h"
#include "pipeline/Renderer.h"
#include "shading/Lighting.h"
#include "test_util.h"

namespace {

constexpr int kMinNonBlackPixels = 500;
constexpr std::size_t kMinDistinctColors = 8;
constexpr int kMinChannelRange = 2;

struct CoverageStats {
    int nonBlack = 0;
    std::set<std::uint32_t> colors;
    int minimum[3] = {256, 256, 256};
    int maximum[3] = {-1, -1, -1};
};

int channelOf(std::uint32_t packed, int index) {
    return static_cast<int>((packed >> (8 * (2 - index))) & 0xFFU);
}

CoverageStats analyzeCoverage(const cg::MemoryCanvas& canvas) {
    CoverageStats stats;
    for (int y = 0; y < canvas.height(); ++y) {
        for (int x = 0; x < canvas.width(); ++x) {
            const std::uint32_t color = canvas.pixelAt(x, y);
            if (color == cg::kBlack) {
                continue;
            }
            ++stats.nonBlack;
            stats.colors.insert(color);
            for (int channel = 0; channel < 3; ++channel) {
                const int value = channelOf(color, channel);
                if (value < stats.minimum[channel]) {
                    stats.minimum[channel] = value;
                }
                if (value > stats.maximum[channel]) {
                    stats.maximum[channel] = value;
                }
            }
        }
    }
    return stats;
}

int maxChannelRange(const CoverageStats& stats) {
    int range = 0;
    for (int channel = 0; channel < 3; ++channel) {
        if (stats.maximum[channel] < 0) {
            continue;
        }
        const int channelRange = stats.maximum[channel] - stats.minimum[channel];
        if (channelRange > range) {
            range = channelRange;
        }
    }
    return range;
}

void modelos_reais_tem_cobertura_minima() {
    cg::Camera camera;
    cg::Lighting lighting;
    std::string error;
    CHECK(cg::loadCameraFromFile("camera/camera.txt", camera, error));
    CHECK(cg::loadLightingFromFile("lighting/lighting.txt", lighting, error));

    camera.d = 20.0F;

    const char* paths[] = {"models/triangulo.byu", "models/piramide.byu", "models/maca.byu",
                           "models/maca2.byu",     "models/vaso.byu",     "models/calice2.byu"};

    for (const char* path : paths) {
        cg::Mesh mesh;
        const bool loaded = cg::loadMeshFromFile(path, mesh, error);
        CHECK(loaded);
        if (!loaded) {
            continue;
        }

        cg::MemoryCanvas canvas{800, 600};
        cg::renderMesh(mesh, camera, lighting, canvas);

        const CoverageStats stats = analyzeCoverage(canvas);
        const int range = maxChannelRange(stats);

        std::cout << path << ": nao pretos=" << stats.nonBlack
                  << ", cores=" << stats.colors.size() << ", faixa=" << range << "\n";

        CHECK(stats.nonBlack >= kMinNonBlackPixels);
        CHECK(stats.colors.size() >= kMinDistinctColors);
        CHECK(range >= kMinChannelRange);
    }
}

}

int main() {
    modelos_reais_tem_cobertura_minima();

    return test::failureCount() == 0 ? 0 : 1;
}
