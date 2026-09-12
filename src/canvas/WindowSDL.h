#pragma once

#include <cstdint>
#include <string>

#include "canvas/Canvas.h"

struct SDL_Window;
struct SDL_Renderer;

namespace cg {

class WindowSDL final : public Canvas {
public:
    WindowSDL(const std::string& title, int width, int height);
    ~WindowSDL() override;

    WindowSDL(const WindowSDL&) = delete;
    WindowSDL& operator=(const WindowSDL&) = delete;

    bool isValid() const;

    int width() const override;
    int height() const override;

    void clear(std::uint32_t color) override;
    void drawPixel(int x, int y, std::uint32_t color) override;

    void present();
    void toggleFullscreen();

private:
    void syncSize();

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    bool fullscreen_ = false;
};

}
