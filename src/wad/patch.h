#pragma once
#include "structures.h"
#include "wad.h"
#include "../utils.h"

class PatchImage {
public:
    PatchImage(WADFile& file, WAD::FileLump imageLump, std::vector<uint8_t>& colorPalette);

    uint16_t getWidth() const { return m_width; }
    uint16_t getHeight() const { return m_height; }
    std::vector<uint8_t>& getPixels() { return m_pixels; }

private:
    uint16_t m_width, m_height, m_top, m_left;
    std::vector<uint8_t> m_pixels;

    void readColumns(std::ifstream& stream, uint32_t base, std::vector<uint32_t>& offsets, std::vector<uint8_t>& palette, std::vector<uint8_t>& dest);
};