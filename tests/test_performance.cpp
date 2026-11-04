#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "canvas/MemoryCanvas.h"
#include "mesh/Mesh.h"
#include "pipeline/Camera.h"
#include "pipeline/Renderer.h"
#include "shading/Lighting.h"
#include "test_util.h"

namespace {

constexpr int kWarmupRuns = 2;
constexpr int kMeasuredRuns = 10;
constexpr double kBudgetMilliseconds = 100.0;

double median(std::vector<double> samples) {
    std::sort(samples.begin(), samples.end());
    const std::size_t middle = samples.size() / 2;
    if (samples.size() % 2 == 0) {
        return (samples[middle - 1] + samples[middle]) / 2.0;
    }
    return samples[middle];
}

void mede_desempenho_dos_modelos() {
    cg::Camera camera;
    cg::Lighting lighting;
    std::string error;
    const bool cameraLoaded = cg::loadCameraFromFile("camera/camera.txt", camera, error);
    const bool lightingLoaded = cg::loadLightingFromFile("lighting/lighting.txt", lighting, error);
    CHECK(cameraLoaded);
    CHECK(lightingLoaded);
    if (!cameraLoaded || !lightingLoaded) {
        return;
    }

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
        for (int run = 0; run < kWarmupRuns; ++run) {
            cg::renderMesh(mesh, camera, lighting, canvas);
        }

        std::vector<double> samples;
        samples.reserve(kMeasuredRuns);
        for (int run = 0; run < kMeasuredRuns; ++run) {
            const auto start = std::chrono::steady_clock::now();
            cg::renderMesh(mesh, camera, lighting, canvas);
            const auto end = std::chrono::steady_clock::now();
            samples.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        }

        const auto [minimum, maximum] = std::minmax_element(samples.begin(), samples.end());
        const double mediana = median(samples);
        std::cout << path << ": mediana=" << mediana << " ms, min=" << *minimum
                  << " ms, max=" << *maximum << " ms\n";

        CHECK(mediana <= kBudgetMilliseconds);
    }
}

}

int main() {
    mede_desempenho_dos_modelos();

    return test::failureCount() == 0 ? 0 : 1;
}
