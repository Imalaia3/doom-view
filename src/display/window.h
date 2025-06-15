#pragma once
#include <SDL2/SDL.h>
#include <exception>
#include <assert.h>

class SDLWindow {
public:
    SDLWindow(uint32_t width, uint32_t height);

    ~SDLWindow() {
        SDL_DestroyTexture(m_screenTexture);
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

    // Returns pixel handle
    void* renderBegin();
    
    inline void putPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint32_t b, void *pixels) {
        uint32_t* data = (uint32_t*)pixels;
        data[y*m_width + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
    }
    
    // Extremely Fast Line Algorithm (Copyright 2001-2, By Po-Han Lin)
    void drawLine(int x, int y, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, void* pixels);

    void drawRectFilled(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b, void* pixels);
    
    void renderEnd() { SDL_UnlockTexture(m_screenTexture); }
    
    void updateWindow();

    void clearRenderer(uint8_t r, uint8_t g, uint8_t b) {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, 0xFF);
        SDL_RenderClear(m_renderer);
    }

private:
    uint32_t m_width, m_height;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_screenTexture;
};