#include <stdio.h>
#include <cstring>

#include "wad/wad.h"
#include "wad/map.h"
#include "wad/patch.h"
#include "display/tga.h"
#include "display/window.h"

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

    TGAWriter bossWriter(41, 73);
    for (size_t i = 0; i < 14; i++) {
        auto playpal = wad.getColorPalette(i);
        PatchImage bossImage(wad, wad.getLumpByName("BOSSA1"), playpal);
        bossWriter.putVectorRGB(bossImage.getPixels());
        std::string name = "playpal" + std::to_string(i) + ".tga";
        bossWriter.writeToFile(name.c_str());    
    }

    WADMap e1m1(wad, wad.getLumpByName("E1M1"));
    auto verts = e1m1.getVertices();
    auto ldefs = e1m1.getLinedefs();
    auto player = e1m1.findThingByType(Thing::ThingType::PLAYERONE);
    

    SDLWindow win(800,800);
    auto drawPixels = win.renderBegin();

    // In order to make the level fit some scaling must be applied
    auto adjustX = [](float x) {
        auto nx = (uint32_t)(x / 10.0) + 200;
        nx = (nx < 800) ? nx : 799;
        return (nx > 0) ? nx : 0;
    };
    auto adjustY = [](float y) {
        auto nx = (uint32_t)(y / 10.0) + 800;
        nx = (nx < 800) ? nx : 799;
        return (nx > 0) ? nx : 0;
    };

    for (auto &&def : ldefs) {
        auto v1 = verts[def.v1];
        auto v2 = verts[def.v2];
        win.drawLine(adjustX(v1.x), adjustY(v1.y), adjustX(v2.x), adjustY(v2.y), 0xFF, 0xFF, 0xFF, drawPixels);
    }

    win.drawRectFilled(adjustX(player.position.x)-2, adjustY(player.position.y)-2, 4, 4, 0x00, 0xFF, 0xFF, drawPixels);

    win.renderEnd();
    win.updateWindow();
    SDL_Delay(5000);

    return 0;
}
