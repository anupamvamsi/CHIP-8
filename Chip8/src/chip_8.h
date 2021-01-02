#ifndef CHIP8_CHIP_8_H

#define CHIP8_CHIP_8_H

#include <cstdint>
#include <random>

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;  // 16 chars (0 to F), 5 Bytes each
const unsigned int FONTSET_START_ADDRESS = 0x50;  // from reserved mem

class Chip8 {
 public:
  // variables - uniform initialization :: uint8, uint16 = chars

  uint8_t registers8_16[16]{};        // 8-bit (1 Byte) regs - 16
  uint8_t memory8_4kb[4096]{};        // 8-bit mem spots - 4096
  uint16_t index16{};                 // 16-bit IR
  uint16_t pc16{};                    // 16-bit PC
  uint16_t stack16_16[16]{};          // 16-lvl stack (for PC vals)
  uint8_t sp8{};                      // 8-bit SP
  uint8_t delay_timer8{};             // 8-bit delay timer
  uint8_t sound_timer8{};             // 8-bit sound timer
  uint8_t keypad8_16[16]{};           // 8-bit keys - 16 (0 to F)
  uint32_t video32_64_32[64 * 32]{};  // 32-bit (64 x 32 - W x H) displ. mem
  uint16_t opcode16;                  // 16-bit opc (e.g., 0x7522)

  // functions
  Chip8();  // default ctor

  void LoadRom(char const* rom);  // load ROM instrucns to mem before executn
  void Cycle();

 private:
  void Table0();
  void Table8();
  void TableE();
  void TableF();

  // 34 INSTRUCTIONS OF CHIP-8

  void Op_NULL();  // 00

  void Op_00E0();  // 01
  void Op_00EE();  // 02

  // #
  void Op_1nnn();  // 03
  void Op_2nnn();  // 04
  void Op_3xnn();  // 05
  void Op_4xnn();  // 06
  void Op_5xy0();  // 07
  void Op_6xnn();  // 08
  void Op_7xnn();  // 09

  // 8
  void Op_8xy0();  // 10
  void Op_8xy1();  // 11
  void Op_8xy2();  // 12
  void Op_8xy3();  // 13
  void Op_8xy4();  // 14
  void Op_8xy5();  // 15
  void Op_8xy6();  // 16
  void Op_8xy7();  // 17
  void Op_8xyE();  // 18

  // 9
  void Op_9xy0();  // 19

  // a
  void Op_Annn();  // 20
  void Op_Bnnn();  // 21
  void Op_Cxnn();  // 22
  void Op_Dxyn();  // 23

  // E
  void Op_Ex9E();  // 24
  void Op_ExA1();  // 25

  // F
  void Op_Fx07();  // 26
  void Op_Fx0A();  // 27
  void Op_Fx15();  // 28
  void Op_Fx18();  // 29
  void Op_Fx1E();  // 30
  void Op_Fx29();  // 31
  void Op_Fx33();  // 32
  void Op_Fx55();  // 33
  void Op_Fx65();  // 34

  std::mt19937 RandomGen;                              // Random engine
  std::uniform_int_distribution<uint8_t> random_byte;  // For a range of randnum

  // using (Chip8::*Chip8Func)() = void;  // Why can't I use this?
  // using Chip8Func = void;              // Should be used like this?

  typedef void (Chip8::*Chip8Func)();

  Chip8Func table[0xF + 1]{&Chip8::Op_NULL};
  Chip8Func table0[0xE + 1]{&Chip8::Op_NULL};
  Chip8Func table8[0xE + 1]{&Chip8::Op_NULL};
  Chip8Func tableE[0xE + 1]{&Chip8::Op_NULL};
  Chip8Func tableF[0x65 + 1]{&Chip8::Op_NULL};
};

#endif  // CHIP8_CHIP_8_H

/*
  ----------------------
  INSTRUCTIONS REFERENCE
  ----------------------

  // -
  NULL  00) Do nothing

  // 0
  00E0  01) CLS - Clear screen
  00EE  02) RET - Return from a Subroutine


  // #
  1nnn  03) JMP - Jump to location nnn
  2nnn  04) CALL - Execute subroutine @nnn
  3xnn  05) Skip next instruction if Vx = nn
  4xnn  06) Skip next instruction if Vx != nn
  5xy0  07) Skip next instruction if Vx = Vy
  6xnn  08) Set Vx = nn
  7xnn  09) Set Vx = Vx + nn


  // 8
  8xy0  10) Set Vx = Vy
  8xy1  11) Set Vx = Vx OR  Vy
  8xy2  12) Set Vx = Vx AND Vy
  8xy3  13) Set Vx = Vx XOR Vy
  8xy4  14) Set Vx = Vx + Vy; Vf = 01 if Carry, else Vf = 00
  8xy5  15) Set Vx = Vx - Vy; Vf = 00 if borrow, else Vf = 01
  8xy6  16) Vy >> Vx; Shift Right Vy (1-bit) and store in Vx;
            Don't modify Vy; Store Vy's LSB in Vf before shifting

  8xy7  17) Set Vx = Vy - Vx; Vf = 00 if borrow, else Vf = 01

  8xyE  18) Vx << Vy; Shift Left Vy (1-bit) and store in Vx; Don't
            modify Vy; Store Vy's MSB in Vf before shifting


  // 9
  9xy0  19) Skip next instruction if Vx != Vy


  // a
  Annn  20) Set index16 = nnn (Instruction Register - IR)
  Bnnn  21) Jump to address (nnn + V0)
  Cxnn  22) Set Vx = random number with mask of nn
  Dxyn  23) Draw sprite at (Vx, Vy) with n Bytes of sprite data
            starting at the address stored in `index16`; Set Vf = 01
            if any set pixels are unset; Vf = 00 otherwise


  // E
  Ex9E  24) Skip next instrucn if key with value in Vx is pressed
  ExA1  25) Skip next instrucn if key with value in Vx is ~pressed


  // F
  Fx07  26) Set Vx = delay_timer8
  Fx0A  27) Wait for key press, store key value in Vx
  Fx15  28) Set delay_timer8 = Vx
  Fx18  29) Set sound_timer8 = Vx
  Fx1E  30) Set index16 = Vx + index16
  Fx29  31) Set index16 = address(sprite => corresp. 0x num in Vx)
  Fx33  32) Store Binary Coded Decimal equivalent of value in Vx
            @addresses `index16`, (`index16` + 1), (`index16` + 2)

  Fx55  33) Store values of [V0 to Vx] in memory starting @addr.
            `index16`; Set `index16` = `index16` + x + 1 after storing

  Fx65  34) Fill [V0 to Vx] with values in memory starting @addr.
            `index16`; Set `index16` = `index16` + x + 1 after filling

*/