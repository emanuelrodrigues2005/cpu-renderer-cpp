#include <cstdlib>
#include <iostream>
#include <string>

#include <SDL2/SDL.h>

#include "Mesh.h"
#include "WindowSDL.h"

namespace {

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
constexpr const char* kDefaultModelPath = "models/piramide.byu";

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

}

int main(int argc, char** argv) {
    bool smokeTest = false;
    bool testPattern = false;
    bool showInfo = false;
    std::string modelPath = kDefaultModelPath;
    bool modelPathGiven = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--smoke") {
            smokeTest = true;
        } else if (arg == "--test-pattern") {
            testPattern = true;
        } else if (arg == "--info") {
            showInfo = true;
        } else if (!modelPathGiven) {
            modelPath = arg;
            modelPathGiven = true;
        } else {
            std::cerr << "argumento desconhecido: " << arg << "\n";
            return EXIT_FAILURE;
        }
    }

    if (showInfo) {
        return printMeshInfo(modelPath);
    }

    const bool showTestPattern = testPattern || smokeTest;

    cg::WindowSDL window("cpu-renderer-cpp", kWindowWidth, kWindowHeight);
    if (!window.isValid()) {
        return EXIT_FAILURE;
    }

    window.clear(cg::kBlack);
    if (showTestPattern) {
        drawTestPattern(window);
    }
    window.present();

    if (smokeTest) {
        return EXIT_SUCCESS;
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                const SDL_Keycode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE || key == SDLK_q) {
                    running = false;
                } else if (key == SDLK_r) {
                    window.clear(cg::kBlack);
                    if (showTestPattern) {
                        drawTestPattern(window);
                    }
                    window.present();
                }
            }
        }
        SDL_Delay(16);
    }

    return EXIT_SUCCESS;
}
