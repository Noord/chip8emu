#ifndef CHIP_8_DEBUGGER
#define CHIP_8_DEBUGGER

#include "chip8.hpp"

struct Debugger{
    static void dump_screen(CPU& c);
    static void dump_memory(CPU& c);
};

#endif