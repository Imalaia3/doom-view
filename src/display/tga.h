#pragma once
#include <fstream>
#include <vector>
#include <stdint.h>
#include <assert.h>

class TGAWriter {
public:
    TGAWriter(uint16_t imgWidth, uint16_t imgHeight) : m_width(imgWidth), m_height(imgHeight) {
        m_pixels.resize(m_width*m_height*3);
    }
    
    void putPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
    
    void putVectorBGR(std::vector<uint8_t>& source) {
        assert(source.size() == m_pixels.size());
        m_pixels = source;
    }
    void putVectorRGB(std::vector<uint8_t>& source) {
        assert(source.size() == m_pixels.size());
        m_pixels = source;
        for (size_t i = 0; i < m_pixels.size(); i += 3) { std::swap(m_pixels[i], m_pixels[i + 2]); }
    }
    void writeToFile(const char* filename);

    uint16_t getWidth() const { return m_width; }
    uint16_t getHeight() const { return m_height; }

private:
    struct TGAHeader {
        uint8_t idLength;
        uint8_t colorMapType;
        uint8_t imageType;
        struct colorMapSpec {
            uint16_t firstEntryIdx;
            uint16_t colorMapLength;
            uint8_t bitsPerColorEntry;
        } __attribute__((packed)) colorMapSpec;

        struct imageSpec {
            uint16_t xOrigin;
            uint16_t yOrigin;
            uint16_t width;
            uint16_t height;
            uint8_t pixelDepth;
            uint8_t imgDescriptor;
        } __attribute__((packed)) imageSpec;
    } __attribute__((packed));

    enum ImageType {
        NoImageData = 0,
        UncompressedColorMap = 1,
        UncompressedTrueColor = 2,
        UncompressedGrayscale = 3,
        RLEColorMap = 9,
        RLETrueColor = 10,
        RLEGrayScale = 11
    };

    uint16_t m_width, m_height;
    std::vector<uint8_t> m_pixels;

    TGAHeader buildHeader();
};