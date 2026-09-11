#include "WindowSDL.h"

#include <SDL2/SDL.h>

namespace cg {

namespace {

void setDrawColor(SDL_Renderer* renderer, std::uint32_t color) {
    const auto r = static_cast<std::uint8_t>((color >> 16) & 0xFF);
    const auto g = static_cast<std::uint8_t>((color >> 8) & 0xFF);
    const auto b = static_cast<std::uint8_t>(color & 0xFF);
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
}

}  // namespace

WindowSDL::WindowSDL(const std::string& title, int width, int height)
    : width_(width), height_(height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height, SDL_WINDOW_SHOWN);
    if (window_ == nullptr) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr) {
        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
    }
    if (renderer_ == nullptr) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    }
}

WindowSDL::~WindowSDL() {
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

bool WindowSDL::isValid() const { return window_ != nullptr && renderer_ != nullptr; }

int WindowSDL::width() const { return width_; }

int WindowSDL::height() const { return height_; }

void WindowSDL::clear(std::uint32_t color) {
    setDrawColor(renderer_, color);
    SDL_RenderClear(renderer_);
}

void WindowSDL::drawPixel(int x, int y, std::uint32_t color) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return;
    }
    setDrawColor(renderer_, color);
    SDL_RenderDrawPoint(renderer_, x, y);
}

void WindowSDL::present() { SDL_RenderPresent(renderer_); }

}  // namespace cg
