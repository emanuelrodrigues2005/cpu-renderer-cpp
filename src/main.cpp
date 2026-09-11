#include <cstdlib>
#include <cstring>

#include <SDL2/SDL.h>

#include "WindowSDL.h"

namespace {

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;

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

}  // namespace

int main(int argc, char** argv) {
    bool smokeTest = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--smoke") == 0) {
            smokeTest = true;
        }
    }

    cg::WindowSDL window("cpu-renderer-cpp", kWindowWidth, kWindowHeight);
    if (!window.isValid()) {
        return EXIT_FAILURE;
    }

    window.clear(cg::kBlack);
    drawTestPattern(window);
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
                    drawTestPattern(window);
                    window.present();
                }
            }
        }
        SDL_Delay(16);
    }

    return EXIT_SUCCESS;
}
