#include "window.h"
#include <stdexcept>

SDLWindow::SDLWindow(uint32_t width, uint32_t height) : m_width(width), m_height(height) {
    SDL_Init(SDL_INIT_VIDEO);
    m_window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_SHOWN);
    if (!m_window)
        throw std::runtime_error("Failed to create SDL2 window");
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer)
        throw std::runtime_error("Failed to create SDL2 renderer");
    m_screenTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
    if (!m_screenTexture)
        throw std::runtime_error("Failed to create SDL2 texture");
}

void* SDLWindow::renderBegin()
{
    void* ptr;
    int pitch; // The SDL2 source code dereferences it directly so no nullptr
    SDL_LockTexture(m_screenTexture, NULL, &ptr, &pitch); // if != 0 fail
    return ptr;
}

void SDLWindow::putPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint32_t b, void *pixels) {
    uint32_t* data = (uint32_t*)pixels;
    data[y*m_width + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
}

void SDLWindow::updateWindow() {
    SDL_RenderCopy(m_renderer, m_screenTexture, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}