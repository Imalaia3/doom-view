#pragma once
#include <stdint.h>
#include "../math/bbox.h"

namespace WAD {
    struct FileLump {
        uint32_t filepos;
        uint32_t size;
        char name[8];
    } __attribute__((packed));

    struct PatchHeader {
        uint16_t width;
        uint16_t height;
        uint16_t leftOffset;
        uint16_t topOffset;
    } __attribute__((packed));

    struct ThingEntry {
        int16_t x;
        int16_t y;
        int16_t angleDeg; // Degrees
        int16_t type;
        int16_t flags;
    } __attribute__((packed));

    struct LinedefEntry {
        int16_t v1;
        int16_t v2;
        int16_t flags;
        int16_t special;
        int16_t tag;
        int16_t frontSidedef;
        int16_t backSidedef;
    } __attribute__((packed));

    struct SidedefEntry {
        int16_t xOffset;
        int16_t yOffset;
        char upperTex[8];
        char lowerTex[8];
        char middleTex[8];
        int16_t lookatSector;
    } __attribute__((packed));

    struct SectorEntry {
        int16_t floorHeight;
        int16_t ceilHeight;
        char floorTex[8];
        char ceilTex[8];
        int16_t lightLvl;
        int16_t special;
        int16_t tag;
    } __attribute__((packed));

    struct NodeEntry {
        int16_t splitterX;
        int16_t splitterY;
        int16_t splitterDeltaX;
        int16_t splitterDeltaY;
        int16_t rightBoundingBox[4];
        int16_t leftBoundingBox[4];
        int16_t rightChild;
        int16_t leftChild;

        Math::BoundingBox getLeftBox()  {
            return Math::BoundingBox(leftBoundingBox[2], leftBoundingBox[1], 
                leftBoundingBox[3], leftBoundingBox[0]);
        }
        Math::BoundingBox getRightBox()  {
            return Math::BoundingBox(rightBoundingBox[2], rightBoundingBox[1], 
                rightBoundingBox[3], rightBoundingBox[0]);
        }

    } __attribute__((packed));

    struct SegEntry {
        int16_t vbeg;
        int16_t vend;
        int16_t angleDeg;
        int16_t linedef;
        int16_t dir;
        int16_t offset;
    } __attribute__((packed));
    
    struct SubsectorEntry {
        int16_t segCount;
        int16_t firstSeg;
    } __attribute__((packed));
}