#include <stdio.h>
#include <cstring>

#include "wad/wad.h"
#include "display/tga.h"

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("Syntax: ./viewer [file.wad]\n");
        return 1;
    }

    WADFile wad(argv[1]);
    printf("%s: %i lumps\n", argv[1], wad.getLumpCount());
    for (const auto &lump : wad.getLumps()) {
        char string[9]; std::memcpy(string, lump.name, 8);
        string[8] = '\0';
        printf("Name: %s\n", string);
    }

    TGAWriter writer(256, 256);
    for (size_t i = 0; i < 256; i++) {
        for (size_t j = 0; j < 256; j++) {
            writer.putPixel(i, j, i, j, 0);
        }
    }
    writer.putPixel(0, 0, 0, 0, 255);
    writer.writeToFile("output.tga");
    
    return 0;
}
