#include "patch.h"

PatchImage::PatchImage(WADFile &file, WAD::FileLump imageLump, std::vector<uint8_t> &colorPalette) {
    auto& stream = file.getStream();
    stream.seekg(imageLump.filepos, std::ios_base::beg);
    
    WAD::PatchHeader header{};
    Utils::streamRead(&header, sizeof(WAD::PatchHeader), stream);
    m_width = header.width;
    m_height = header.height;
    m_left = header.leftOffset;
    m_top = header.topOffset;

    printf("Resolution: %ix%i\n", header.width, header.height);

    std::vector<uint32_t> columnOffsets(m_width);
    Utils::streamRead(columnOffsets.data(), m_width * sizeof(uint32_t), stream);
    m_pixels.resize(m_width * m_height * 3);
    readColumns(stream, imageLump.filepos, columnOffsets, colorPalette, m_pixels);
}

void PatchImage::readColumns(std::ifstream &stream, uint32_t base, std::vector<uint32_t> &offsets, std::vector<uint8_t> &palette, std::vector<uint8_t> &dest) {
    for (uint32_t x = 0; x < offsets.size(); x++) {
        stream.seekg(base + offsets[x], std::ios_base::beg);
        
        uint8_t padding;
        uint8_t topDelta{}; // Also terminator
        while (topDelta != 0xFF) {
            Utils::streamRead(&topDelta, 1, stream);
            if(topDelta == 0xFF)
                break;

            uint8_t length{}; Utils::streamRead(&length, 1, stream);
            Utils::streamRead(&padding, 1, stream);

            for (size_t y = 0; y < length; y++) {
                uint8_t paletteOffset{}; Utils::streamRead(&paletteOffset, 1, stream);
                uint32_t off = ((y+topDelta) * m_width + x) * 3;
                dest[off + 0] = palette[paletteOffset*3 + 0];
                dest[off + 1] = palette[paletteOffset*3 + 1];
                dest[off + 2] = palette[paletteOffset*3 + 2];
            }
            Utils::streamRead(&padding, 1, stream);
        }
    }
}