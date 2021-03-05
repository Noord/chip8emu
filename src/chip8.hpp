#ifndef CHIP_8
#define CHIP_8

#include "types.h"

struct CPU {
public:
    Word pc = 0x200; // 16 bit pc
    Word opcode = 0; // 16 bit opcode
    Word I = 0; // 12 bit address
    Byte sp = 0; // 8 bit sp
    Byte V[0x10] = {0,}; // registers
    Word stack[0x10] = {0,}; // stack
    Byte key[0x10] = {0,}; // keypad

    Byte delay_timer = 0; // delay
    Byte sound_timer = 0; // timer
public:
    Byte memory[0xFFF] = {0,}; // memory
    Byte screen[0x800] = {0,}; // screen

    CPU(Byte fonts[0x50]);    
    void tick();
    void run(std::string game);
    void load(std::string& path);
    void failure_exit(std::string message, int code);
    void _0(); void _1(); void _2(); void _3();
    void _4(); void _5(); void _6(); void _7();
    void _8(); void _9(); void _a(); void _b();
    void _c(); void _d(); void _e(); void _f();
};
#endif