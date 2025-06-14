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

    // BOSSA1
    // PatchImage bossImage(wad, wad.getLumpByName("BOSSA1"));

    TGAWriter paletteWriter(16, 16);
    for (size_t i = 0; i < 14; i++) {
        auto playpal = wad.getColorPalette(i);
        paletteWriter.putVectorRGB(playpal);
        std::string name = "playpal" + std::to_string(i) + ".tga";
        paletteWriter.writeToFile(name.c_str());
    }
    

    return 0;
}
