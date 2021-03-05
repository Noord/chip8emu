#include <iostream>
#include <iomanip>
#include "debugger.hpp"
#include "chip8.hpp"

void Debugger::dump_memory(CPU& c){
    std::cout << "     ";
    for (int i = 0; i < 0x10; i++)
        std::cout << std::setw(2) << std::hex << +(i) << " ";
    std::cout << std::endl;

    for (int i = 0; i < 0x100; i++){
        std::cout << std::setw(3) << std::hex << +(i*16) << "| ";
        
        for (int j = 0; j < 0x10; j++)
            std::cout << std::setw(2) << std::hex << +c.memory[i * 0x10 + j] << " ";
        std::cout << std::endl;
    }
}

void Debugger::dump_screen(CPU& c){
    for (int i = 0; i < 0x20; i++){
        for (int j = 0; j < 0x40; j++)
            std::cout << std::hex << +c.screen[i * 0x40 + j] << "";
        std::cout << std::endl;
    }
}