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


void SDLWindow::drawLine(int x, int y, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, void *pixels) {
    bool yLonger=false;
    int incrementVal, endVal;
    int shortLen=y2-y;
    int longLen=x2-x;
    if (abs(shortLen)>abs(longLen)) {
        int swap=shortLen;
        shortLen=longLen;
        longLen=swap;
        yLonger=true;
    }
    endVal=longLen;
    if (longLen<0) {
        incrementVal=-1;
        longLen=-longLen;
    } else incrementVal=1;
    int decInc;
    if (longLen==0) decInc=0;
    else decInc = (shortLen << 16) / longLen;
    int j=0;
    if (yLonger) {	
        for (int i=0;i!=endVal;i+=incrementVal) {
            putPixel(x+(j >> 16),y+i, r, g, b, pixels);	
            j+=decInc;
        }
    } else {
        for (int i=0;i!=endVal;i+=incrementVal) {
            putPixel(x+i,y+(j >> 16), r, g, b, pixels);
            j+=decInc;
        }
    }
}

void SDLWindow::drawRectFilled(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b, void *pixels) {
    assert(x + w <= m_width);
    assert(y + h <= m_height);
    for (uint32_t i = y; i < y + h; i++) {
        for (uint32_t j = x; j < x + w; j++) {
            putPixel(j, i, r, g, b, pixels);
        }
    }
}

void SDLWindow::updateWindow() {
    SDL_RenderCopy(m_renderer, m_screenTexture, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}