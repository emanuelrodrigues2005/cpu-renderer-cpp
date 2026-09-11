#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

#include <SDL2/SDL.h>

#include "Camera.h"
#include "Mesh.h"
#include "Renderer.h"
#include "WindowSDL.h"

namespace {

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
constexpr const char* kDefaultModelPath = "models/piramide.byu";
constexpr const char* kDefaultCameraPath = "camera/presets/piramide.txt";

struct ModelPreset {
    const char* model;
    const char* camera;
};

const ModelPreset kModelPresets[] = {
    {"models/triangulo.byu", "camera/presets/triangulo.txt"},
    {"models/piramide.byu", "camera/presets/piramide.txt"},
    {"models/maca.byu", "camera/presets/maca.txt"},
    {"models/maca2.byu", "camera/presets/maca2.txt"},
    {"models/vaso.byu", "camera/presets/vaso.txt"},
    {"models/calice2.byu", "camera/presets/calice2.txt"},
};

void drawTestPattern(cg::WindowSDL& window) {
    for (int x = 0; x < window.width(); ++x) {
        window.drawPixel(x, 0, cg::kWhite);
        window.drawPixel(x, window.height() - 1, cg::kWhite);
    }
    for (int y = 0; y < window.height(); ++y) {
        window.drawPixel(0, y, cg::kWhite);
        window.drawPixel(window.width() - 1, y, cg::kWhite);
    }
    for (int i = 0; i < window.width() && i < window.height(); ++i) {
        window.drawPixel(i, i, cg::kWhite);
        window.drawPixel(window.width() - 1 - i, i, cg::kWhite);
    }
}

std::string countLabel(std::size_t count, const char* singular, const char* plural) {
    return std::to_string(count) + " " + (count == 1 ? singular : plural);
}

int printMeshInfo(const std::string& path) {
    cg::Mesh mesh;
    std::string error;
    if (!cg::loadMeshFromFile(path, mesh, error)) {
        std::cerr << "erro: " << error << "\n";
        return EXIT_FAILURE;
    }

    std::cout << path << ": " << countLabel(mesh.vertices.size(), "vértice", "vértices") << ", "
              << countLabel(mesh.triangles.size(), "triângulo", "triângulos") << "\n";
    return EXIT_SUCCESS;
}

bool loadModel(const std::string& path, cg::Mesh& mesh) {
    std::string error;
    if (!cg::loadMeshFromFile(path, mesh, error)) {
        std::cerr << "erro: " << error << "\n";
        return false;
    }
    return true;
}

bool loadCamera(const std::string& path, cg::Camera& camera) {
    std::string error;
    if (!cg::loadCameraFromFile(path, camera, error)) {
        std::cerr << "erro: " << error << "\n";
        return false;
    }
    return true;
}

}

int main(int argc, char** argv) {
    bool smokeTest = false;
    bool testPattern = false;
    bool showInfo = false;
    std::string modelPath = kDefaultModelPath;
    std::string cameraPath = kDefaultCameraPath;
    int positionalCount = 0;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--smoke") {
            smokeTest = true;
        } else if (arg == "--test-pattern") {
            testPattern = true;
        } else if (arg == "--info") {
            showInfo = true;
        } else if (positionalCount == 0) {
            modelPath = arg;
            ++positionalCount;
        } else if (positionalCount == 1) {
            cameraPath = arg;
            ++positionalCount;
        } else {
            std::cerr << "argumento desconhecido: " << arg << "\n";
            return EXIT_FAILURE;
        }
    }

    if (showInfo) {
        return printMeshInfo(modelPath);
    }

    cg::Mesh mesh;
    if (!loadModel(modelPath, mesh)) {
        return EXIT_FAILURE;
    }
    cg::Camera camera;
    if (!loadCamera(cameraPath, camera)) {
        return EXIT_FAILURE;
    }

    cg::WindowSDL window("cpu-renderer-cpp", kWindowWidth, kWindowHeight);
    if (!window.isValid()) {
        return EXIT_FAILURE;
    }

    cg::RenderMode mode = cg::RenderMode::Filled;
    const auto redraw = [&]() {
        if (testPattern) {
            window.clear(cg::kBlack);
            drawTestPattern(window);
        } else {
            cg::renderMesh(mesh, camera, window, mode);
        }
        window.present();
    };

    redraw();

    if (smokeTest) {
        return EXIT_SUCCESS;
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_WINDOWEVENT &&
                       event.window.event == SDL_WINDOWEVENT_EXPOSED) {
                redraw();
            } else if (event.type == SDL_KEYDOWN) {
                const SDL_Keycode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE || key == SDLK_q) {
                    running = false;
                } else if (key == SDLK_r) {
                    if (loadCamera(cameraPath, camera)) {
                        redraw();
                    }
                } else if (key == SDLK_d) {
                    mode = mode == cg::RenderMode::Points ? cg::RenderMode::Filled
                                                          : cg::RenderMode::Points;
                    redraw();
                } else if (key == SDLK_w) {
                    mode = mode == cg::RenderMode::Wireframe ? cg::RenderMode::Filled
                                                             : cg::RenderMode::Wireframe;
                    redraw();
                } else if (key == SDLK_PLUS || key == SDLK_EQUALS || key == SDLK_KP_PLUS) {
                    camera.d = std::clamp(camera.d * 1.1F, 0.1F, 10000.0F);
                    redraw();
                } else if (key == SDLK_MINUS || key == SDLK_UNDERSCORE || key == SDLK_KP_MINUS) {
                    camera.d = std::clamp(camera.d / 1.1F, 0.1F, 10000.0F);
                    redraw();
                } else if (key >= SDLK_1 && key <= SDLK_6) {
                    const int index = static_cast<int>(key - SDLK_1);
                    if (loadModel(kModelPresets[index].model, mesh) &&
                        loadCamera(kModelPresets[index].camera, camera)) {
                        redraw();
                    }
                }
            }
        }
        SDL_Delay(16);
    }

    return EXIT_SUCCESS;
}
