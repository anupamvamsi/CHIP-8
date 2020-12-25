#include "Chip8.h"

#include <chrono>
#include <fstream>
#include <random>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;  // 16 characters (0 to F), 5 Bytes each
const unsigned int FONTSET_START_ADDRESS = 0x50;  // from reserved mem

uint8_t fontset[FONTSET_SIZE] = {
    // array [16 x 5B]

    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F

};

Chip8::Chip8()
    : RandomGenerator(
          std::chrono::system_clock::now().time_since_epoch().count())

{  // modifying default constructor
  // Initialize program counter
  pc16 = START_ADDRESS;

  // Load fonts into memory
  for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
    memory8_4kb[FONTSET_START_ADDRESS + i] = fontset[i];
  }

  // random_byte: gives a random number between 0 and 255
  random_byte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

void Chip8::LoadRom(char const* filename) {
  // Opens file as a stream of binary, moves the file pointer to the end
  std::ifstream rom(filename, std::ios::binary | std::ios::ate);

  if (rom.is_open()) {
    std::streampos size = rom.tellg();  // Get file size
    char* buffer = new char[size];      // Allocate buffer to hold contents

    rom.seekg(0, std::ios::beg);  // go to beginning of the file
    rom.read(buffer, size);       // fill the buffer by reading
    rom.close();                  // close the ROM

    // Load ROM contents into CHIP-8's memory (starts at 0x200):
    for (long i = 0; i < size; ++i) {
      memory8_4kb[START_ADDRESS + i] = buffer[i];
    }

    delete[] buffer;
  }
}

// THIRTY-FOUR INSTRUCTIONS OF CHIP-8 ************************************ //
void Chip8::Op_00E0() {  // 01) CLS - Clear screen
  memset(video32_64_32, 0, sizeof(video32_64_32));
}

void Chip8::Op_00EE() {  // 02) RET - Return from Subroutine
  --sp8;
  pc16 = stack16_16[sp8];
}

void Op_1nnn() {  // 03) JMP
}

void Op_2nnn() {  // 04) CALL - Execute subroutine @nnn
}

void Op_3xnn() {  // 05) Skip next instruction if Vx = nn
}

void Op_4xnn() {  // 06) Skip next instruction if Vx != nn
}

void Op_5xy0() {  // 07) Skip next instruction if Vx = Vy
}

void Op_6xnn() {  // 08) Set Vx = nn
}

void Op_7xnn() {  // 09) Set Vx = Vx + nn
}

void Op_8xy0() {  // 10) Set Vx = Vy
}

void Op_8xy1() {  // 11) Set Vx = Vx OR  Vy
}

void Op_8xy2() {  // 12) Set Vx = Vx AND Vy
}

void Op_8xy3() {  // 13) Set Vx = Vx XOR Vy
}

void Op_8xy4() {  // 14) Set Vx = Vx + Vy | Vf = 01 if Carry, else Vf = 00
}

void Op_8xy5() {  // 15) Set Vx = Vx - Vy | Vf = 00 if borrow, else Vf = 01
}

void Op_8xy6() {  // 16) Vy >> Vx | Shift Right Vy (1-bit) and store in Vx |
                  // Don't modify Vy | Store Vy's LSB in Vf before shifting
}

void Op_8xy7() {  // 17) Set Vx = Vy - Vx | Vf = 00 if borrow, else Vf = 01
}

void Op_8xyE() {  // 18) Vx << Vy | Shift Left Vy (1-bit) and store in Vx |
                  // Don't modify Vy | Store Vy's MSB in Vf before shifting
}

void Op_9xy0() {  // 19) Skip next instruction if Vx != Vy
}

void Op_Annn() {  // 20) Set index16 = nnn (Instruction Register - IR)
}

void Op_Bnnn() {  // 21) Jump to address (nnn + V0)
}

void Op_Cxnn() {  // 22) Set Vx = random number with mask of nn
}

void Op_Dxyn() {  // 23) Draw sprite at (Vx, Vy) with n Bytes of sprite data
                  // starting at the address stored in `index16` | Set Vf = 01
                  // if any set pixels are unset | Vf = 00 otherwise
}
void Op_Ex9E() {  // 24) Skip next instruction if key with value in Vx is
                  // pressed
}

void Op_ExA1() {  // 25) Skip next instruction if key with value in Vx is
                  // ~pressed
}

void Op_Fx07() {  // 26) Set Vx = delay_timer8
}

void Op_Fx0A() {  // 27) Wait for key press, store key value in Vx
}

void Op_Fx15() {  // 28) Set delay_timer8 = Vx
}

void Op_Fx18() {  // 29) Set sound_timer8 = Vx
}

void Op_Fx1E() {  // 30) Set index16 = Vx + index16
}

void Op_Fx29() {  // 31) Set index16 = address of sprite => corresp. hex digit
                  // in Vx
}

void Op_Fx33() {  // 32) Store Binary Coded Decimal equivalent of value in Vx
                  // @addresses `index16`, (`index16` + 1), (`index16` + 2)
}

void Op_Fx55() {  // 33) Store values of [V0 to Vx] in memory starting @address
                  // `index16` | Set `index16` = `index16` + x + 1 after storing
}

void Op_Fx65() {  // 34) Fill [V0 to Vx] with values in memory starting @address
                  // `index16` | Set `index16` = `index16` + x + 1 after filling
}

// ************************************************************************ //