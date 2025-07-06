#include <stdio.h>
#include <cstring>
#include <functional>

#include "wad/wad.h"
#include "wad/map.h"
#include "wad/patch.h"
#include "display/tga.h"
#include "display/window.h"
#include "renderer/bsp.h"
#include "renderer/overlap.h"

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
    SDLWindow win(1200,800);
    BSPRenderer renderer(e1m1, win);


    renderer.drawFrame();

    OverlapManager mgr;
    auto cb = [](int32_t s, int32_t e) {
        printf("Drawing %i %i\n", s, e);
    };
    mgr.addWall(OverlapManager::Interval(-5, -2), cb);
    mgr.addWall(OverlapManager::Interval(3,4), cb);
    mgr.addWall(OverlapManager::Interval(-2,1), cb);
    mgr.addWall(OverlapManager::Interval(-1,2), cb);
    mgr.addWall(OverlapManager::Interval(7,9), cb);
    mgr.addWall(OverlapManager::Interval(5,7), cb);
    mgr.addWall(OverlapManager::Interval(6,10), cb);
    mgr.addWall(OverlapManager::Interval(7,8), cb);
    mgr.print();

    SDL_Delay(5000);

    return 0;
}
