#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <ctime>
#include "chip8.hpp"
#include "debugger.hpp"

CPU::CPU(Byte fonts[]) {
    srand (time(NULL));
    for (int i = 0; i < 80; i++) 
        memory[i] = fonts[i];
}

void CPU::failure_exit(std::string message, int code){ std::cout << message << std::hex << code <<std::endl; exit(3); }

void CPU::load(std::string& path){
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type file_size = file.tellg();
    std::vector<Byte> buffer(file_size);
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);
    for (int i = 0; i < file_size; i++) memory[i + 512] = buffer[i];
}

void CPU::run(std::string game){
    std::cout << "Loading ROM: " << game << std::endl;
    load(game);
    Sleep(500);
    while(true){
        tick();
        Debugger::dump_screen(*this);
        Sleep(250);
        system("clear");
    }
}

void CPU::tick() {
    opcode = memory[pc] << 8 | memory[pc + 1];
    std::cout << "OPCODE "<<std::hex << +(opcode & 0xF000) << std::endl
              << "SP "<<std::hex << +(opcode) << std::endl
              << "PC IS " << std::hex << +(pc) << std::endl;

    switch(opcode >> 12){
        case 0: _0(); break; case 1: _1(); break; case 2: _2(); break; case 3: _3(); break;
        case 4: _4(); break; case 5: _5(); break; case 6: _6(); break; case 7: _7(); break;
        case 8: _8(); break; case 9: _9(); break; case 10: _a(); break; case 11: _b(); break;
        case 12: _c(); break; case 13: _d(); break; case 14: _e(); break; case 15: _f(); break;
    }
}

void CPU::_0(){
    switch (opcode & 0x000F) { // 00E
        case 0x0000: // 00E0
            for (int i = 0; i < 0x800; ++i) screen[i] = 0;
            pc+=2;
            break;
        case 0x000E: // 00EE
            sp--;
            pc = stack[--sp];
            pc += 2;
            break;
        default: failure_exit("DUNNO", opcode);
    }
}
void CPU::_1(){ pc = opcode & 0x0FFF; } // 1NNN
void CPU::_2(){ stack[sp] = pc, ++sp, pc = opcode & 0x0FFF ;} // 2NNN
void CPU::_3(){ pc += 2 + 2 * (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)); } // 3XNN
void CPU::_4(){ pc += 2 + 2* V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF); } // 4XNN
void CPU::_5(){ pc +=2 + 2 * V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]; } //
void CPU::_6(){ V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF, pc+= 2; } // 6XNN
void CPU::_7(){ V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF, pc+=2; } // 7XNN
void CPU::_9(){ pc += 2 + 2 * V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]; } // 9XY0
void CPU::_a(){ I = opcode & 0x0FFF, pc+=2; } // ANNN
void CPU::_b(){ pc = (opcode & 0x0FFF) + V[0]; } // BNNN
void CPU::_c(){ V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF) , pc+=2; } // CXNN


void CPU::_d(){ // DXYN
    Word x = V[(opcode & 0x0F00) >> 8];
    Word y = V[(opcode & 0x00F0) >> 4];
    Word height = opcode & 0x000F;
    Word pixel;

    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++)
    {
        pixel = memory[I + yline];
        for(int xline = 0; xline < 8; xline++)
        {
            if((pixel & (0x80 >> xline)) != 0)
            {
                if(screen[(x + xline + ((y + yline) * 64))] == 1)  V[0xF] = 1;
                screen[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }
    pc += 2;
}

void CPU::_e(){ // EX
    switch (opcode & 0x00FF) {
        case 0x009E: pc +=  2 + 2 * key[V[(opcode & 0x0F00) >> 8]] != 0; break; // EX9E
        case 0x00A1: pc += 2 * 2 * key[V[(opcode & 0x0F00) >> 8]] == 0; break; // EXA1
        default: failure_exit("DUNNO", opcode);
    }
}

void CPU::_8(){ // 8XY
    switch (opcode & 0x000F) {
        case 0x0000:  V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4], pc+=2; break; // 8XY0
        case 0x0001:  V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4], pc += 2; break; // 8XY1
        case 0x0002:  V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4], pc+=2; break; // 8XY2
        case 0x0003: V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4], pc+=2; break; // 8XY3
        case 0x0004: // 8XY4
            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
            V[0xF] = V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]);
            pc += 2;
            break;
        case 0x0005: // 8XY5
            V[0xF] = !(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]);
            V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0006: // 0x8XY6
            V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
            V[(opcode & 0x0F00) >> 8] >>= 1;
            pc += 2;
            break;
        case 0x0007: // 0x8XY7
            V[0xF] = !(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]);
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x000E: // 0x8XYE
            V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
            V[(opcode & 0x0F00) >> 8] <<= 1;
            pc += 2;
            break;
        default: failure_exit("DUNNO", opcode);
    }
}

// thanks to
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#memmap
// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
// http://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/reports/Chip8.pdf
// http://johnearnest.github.io/Octo/docs/chip8ref.pdf
void CPU::_f(){ // FX__
    switch(opcode & 0x00FF){
        case 0x0007: V[(opcode & 0x0F00) >> 8] = delay_timer, pc+=2; break; // FX07
        case 0x000A: // FX0A
        {
            bool kp = false;
            for(int i = 0; i < 0x10; ++i)
                if(key[i] != 0)
                    V[(opcode & 0x0F00) >> 8] = i,kp = true;;
            if(!kp)
                return;

            pc += 2;
        }
            break;
        case 0x0015: delay_timer = V[(opcode & 0x0F00) >> 8], pc+=2; break; // FX15
        case 0x0018: sound_timer = V[(opcode & 0x0F00) >> 8], pc += 2; break; // FX18
        case 0x001E: // FX1E
            V[0xF] = !(I + V[(opcode & 0x0F00) >> 8] > 0xFFF);
            I += V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x0029: I = V[(opcode & 0x0F00) >> 8] * 0x5, pc += 2; break; // FX29
        case 0x0033: // FX33
            memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
            memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
            pc += 2;
            break;
        case 0x0055: // FX55
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                memory[I + i] = V[i];
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;
        case 0x0065:
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                V[i] = memory[I + i];
            
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;
        default: failure_exit("DUNNO", opcode);
    }
}