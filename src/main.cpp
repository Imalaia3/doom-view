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
    auto nodes = e1m1.getNodes();
    auto rootNode = nodes[nodes.size()-1];
    

    SDLWindow win(1000,800);
    auto drawPixels = win.renderBegin();

    
    // In order to make the level fit some scaling must be applied
    float minX = std::numeric_limits<float>::max(), maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max(), maxY = std::numeric_limits<float>::lowest();
    for (const auto& vert : verts) {
        minX = std::min(minX, vert.x);
        maxX = std::max(maxX, vert.x);
        minY = std::min(minY, vert.y);
        maxY = std::max(maxY, vert.y);
    }

    auto screenX = [minX, maxX](float x) { return ((999) / (maxX - minX)) * (x - minX) + 0; };
    auto screenY = [minY, maxY](float y) { return -((600) / (maxY - minY)) * (y - maxY) + 60; };
    

    for (const auto& def : ldefs) {
        float x1 = screenX(verts[def.v1].x);
        float y1 = screenY(verts[def.v1].y);
        float x2 = screenX(verts[def.v2].x);
        float y2 = screenY(verts[def.v2].y);

        win.drawLine(x1, y1, x2, y2, 0xFF, 0xFF, 0xFF, drawPixels);
    }

    win.drawRectFilled(screenX(player.position.x)-2, screenY(player.position.y), 4, 4, 0x00, 0xFF, 0xFF, drawPixels);
    
    //Back = Left
    win.drawRectHollow(
        screenX(rootNode.leftBoundingBox[2]), screenY(rootNode.leftBoundingBox[0]),
        screenX(rootNode.leftBoundingBox[3]), screenY(rootNode.leftBoundingBox[1]),
        0xFF, 0x00, 0x00, drawPixels
    );
    
    //Front = Right
    win.drawRectHollow(
        screenX(rootNode.rightBoundingBox[2]), screenY(rootNode.rightBoundingBox[0]),
        screenX(rootNode.rightBoundingBox[3]), screenY(rootNode.rightBoundingBox[1]),
        0x00, 0xFF, 0x00, drawPixels
    );

    win.drawLine(
        screenX(rootNode.splitterX), screenY(rootNode.splitterY),
        screenX(rootNode.splitterX) + rootNode.splitterDeltaX, screenY(rootNode.splitterY)+ rootNode.splitterDeltaY,
        0xFF, 0xFF, 0x00, drawPixels
    );

    win.drawRectFilled(screenX(rootNode.splitterX), screenY(rootNode.splitterY), 2,2,0xFF, 0x00, 0xFF, drawPixels);
    

    win.renderEnd();
    win.updateWindow();
    SDL_Delay(5000);

    return 0;
}
