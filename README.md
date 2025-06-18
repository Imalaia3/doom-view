# doom-view
Doom View is a basic DOOM engine renderer that aims to be as accurate to the original drawing algorithms as possible while also having relatively modern C++ code. It can read any WAD (IWAD) that the original game could. For testing purposes the shareware version of DOOM has been supplied (`doom1.wad, v1.9, SHA-1: 5b2e249b9c5133ec987b3ea77596381dc0d6bc1d`).

## Compilation & Execution
### Prerequisites
- GCC/G++ (with C++17 support and AddressSanitizer)
- Terminal with the support for the [find](https://man7.org/linux/man-pages/man1/find.1.html) command.
- [SDL2](https://wiki.libsdl.org/SDL2/FrontPage) 
---
**Notice**:  Currently, there is no release build target for the project, only the debug one.

To compile the `viewer` executable, **use the provided Makefile** by executing the command `make all`.
Then, **to use the program**, run `./viewer [wad file]`. `[wad file]` can be any compatible wad file that begins with the `IWAD` identifier.

## Resources
- [The Doom Wiki](https://doomwiki.org/wiki/Entryway)
- [Game Engine Black Book: DOOM (by Fabien Sanglard)](https://fabiensanglard.net/gebbdoom/)