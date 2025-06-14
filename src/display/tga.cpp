#include "tga.h"

void TGAWriter::putPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
    auto base = (x + y * m_width) * 3;
    assert(base < m_pixels.size());
    m_pixels[base + 0] = b;
    m_pixels[base + 1] = g;
    m_pixels[base + 2] = r;
}

void TGAWriter::writeToFile(const char *filename) {
    std::ofstream stream(filename, std::ios::binary);
    if(stream.fail())
    throw std::runtime_error("Failed to open output stream.");
    
    TGAHeader header = buildHeader();
    assert(m_pixels.size() == m_width * m_height * 3);

    stream.write(reinterpret_cast<char*>(&header), sizeof(TGAHeader));
    stream.write(reinterpret_cast<char*>(m_pixels.data()), m_pixels.size());
    stream.close();
}

TGAWriter::TGAHeader TGAWriter::buildHeader() {
    TGAHeader header{};
    header.idLength = 0;
    header.colorMapType = 0; // No colormap
    header.imageType = ImageType::UncompressedTrueColor;
    
    header.colorMapSpec.firstEntryIdx = 0;
    header.colorMapSpec.colorMapLength = 0;
    header.colorMapSpec.bitsPerColorEntry = 0;

    header.imageSpec.xOrigin = 0;
    header.imageSpec.yOrigin = 0;
    header.imageSpec.width = m_width;
    header.imageSpec.height = m_height;
    header.imageSpec.pixelDepth = 3 * 8;
    header.imageSpec.imgDescriptor = 0b00100000; // From top to bottom rendering
    return header;
}