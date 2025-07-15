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


    // Set all pixels to black. Extremely slow
    void clearPixels(void* pixels) {
        uint32_t* data = (uint32_t*)pixels;
        for (size_t y = 0; y < m_height; y++) {
            for (size_t x = 0; x < m_width; x++) {
                data[y*m_width + x] = 0xFF;
            }
        }
    }

    void verticalScanline(uint32_t x, uint8_t r, uint8_t g, uint8_t b, void* pixels) {
        assert(x < m_width);
        uint32_t* data = (uint32_t*)pixels;
        for (uint32_t y = 0; y < m_height; y++) {
            data[y*m_width + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
        }                
    }
    
    void verticalLine(uint32_t x, uint32_t y1, uint32_t y2, uint8_t r, uint8_t g, uint8_t b, void* pixels) {
        assert(x < m_width);
        if (y2 >= m_height) y2 = m_height - 1;
        if (y1 >= m_height) y1 = m_height - 1;
        uint32_t* data = (uint32_t*)pixels;
        for (uint32_t y = y1; y <= y2; y++) {
            data[y*m_width + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
        }                
    }
    
    // Extremely Fast Line Algorithm (Copyright 2001-2, By Po-Han Lin)
    void drawLine(int x, int y, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, void* pixels);

    void drawRectFilled(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b, void* pixels);
    
    void renderEnd() { SDL_UnlockTexture(m_screenTexture); }
    
    void updateWindow();

    void drawRectHollow(uint32_t lowX, uint32_t lowY, uint32_t highX, uint32_t highY, uint8_t r, uint8_t g, uint8_t b, void* pixels);

    void clearRenderer(uint8_t r, uint8_t g, uint8_t b) {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, 0xFF);
        SDL_RenderClear(m_renderer);
    }

private:
    bool outOfBoundsX(int32_t x) { if (x < 0) return true; if (static_cast<uint32_t>(x) >= m_width) return true; return false; }
    bool outOfBoundsY(int32_t y) { if (y < 0) return true; if (static_cast<uint32_t>(y) >= m_height) return true; return false; }

    uint32_t m_width, m_height;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_screenTexture;
};