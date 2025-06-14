#include <stdio.h>
#include <cstring>

#include "wad/wad.h"

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
    
    return 0;
}
