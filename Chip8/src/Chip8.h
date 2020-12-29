#ifndef CHIP8_CHIP8_H

#define CHIP8_CHIP8_H

#include <cstdint>
#include <random>

class Chip8 {
 public:
  // variables
  // Note: type of initialization here is "uniform initialization". The uint8s
  // and uint16s are treated as chars

  uint8_t registers8_16[16]{};        // 8-bit (1 Byte) registers - 16
  uint8_t memory8_4kb[4096]{};        // 8-bit memory spots - 4096
  uint16_t index16{};                 // 16-bit index register
  uint16_t pc16{};                    // 16-bit program counter
  uint16_t stack16_16[16]{};          // 16 Level Stack (for PC values)
  uint8_t sp8{};                      // 8-bit stack pointer
  uint8_t delay_timer8{};             // 8-bit delay timer
  uint8_t sound_timer8{};             // 8-bit sound timer
  uint8_t keypad8_16[16]{};           // 8-bit keys - 16 (0 to F)
  uint32_t video32_64_32[64 * 32]{};  // 32-bit (64x32) display memory
  uint16_t opcode16;                  // 16-bit opcode (e.g., 0x7522)

  // functions
  Chip8();                             // default constructor
  void LoadRom(char const* filename);  // To load contents (instructions) of ROM
                                       // into memory before execution

 private:
  // THIRTY-FOUR INSTRUCTIONS OF CHIP-8 ------------------------------------- //
  void Op_00E0();  // 01) CLS - Clear screen
  void Op_00EE();  // 02) RET - Return from a Subroutine

  // #
  void Op_1nnn();  // 03) JMP - Jump to location nnn
  void Op_2nnn();  // 04) CALL - Execute subroutine @nnn
  void Op_3xnn();  // 05) Skip next instruction if Vx = nn
  void Op_4xnn();  // 06) Skip next instruction if Vx != nn
  void Op_5xy0();  // 07) Skip next instruction if Vx = Vy
  void Op_6xnn();  // 08) Set Vx = nn
  void Op_7xnn();  // 09) Set Vx = Vx + nn

  // 8
  void Op_8xy0();  // 10) Set Vx = Vy
  void Op_8xy1();  // 11) Set Vx = Vx OR  Vy
  void Op_8xy2();  // 12) Set Vx = Vx AND Vy
  void Op_8xy3();  // 13) Set Vx = Vx XOR Vy
  void Op_8xy4();  // 14) Set Vx = Vx + Vy; Vf = 01 if Carry, else Vf = 00
  void Op_8xy5();  // 15) Set Vx = Vx - Vy; Vf = 00 if borrow, else Vf = 01
  void Op_8xy6();  // 16) Vy >> Vx; Shift Right Vy (1-bit) and store in Vx;
                   // Don't modify Vy; Store Vy's LSB in Vf before shifting
  void Op_8xy7();  // 17) Set Vx = Vy - Vx; Vf = 00 if borrow, else Vf = 01
  void Op_8xyE();  // 18) Vx << Vy; Shift Left Vy (1-bit) and store in Vx; Don't
                   // modify Vy; Store Vy's MSB in Vf before shifting

  // 9
  void Op_9xy0();  // 19) Skip next instruction if Vx != Vy

  // a
  void Op_Annn();  // 20) Set index16 = nnn (Instruction Register - IR)
  void Op_Bnnn();  // 21) Jump to address (nnn + V0)
  void Op_Cxnn();  // 22) Set Vx = random number with mask of nn
  void Op_Dxyn();  // 23) Draw sprite at (Vx, Vy) with n Bytes of sprite data
                   // starting at the address stored in `index16`; Set Vf = 01
                   // if any set pixels are unset; Vf = 00 otherwise

  // E
  void Op_Ex9E();  // 24) Skip next instrucn if key with value in Vx is pressed
  void Op_ExA1();  // 25) Skip next instrucn if key with value in Vx is ~pressed

  // F
  void Op_Fx07();  // 26) Set Vx = delay_timer8
  void Op_Fx0A();  // 27) Wait for key press, store key value in Vx
  void Op_Fx15();  // 28) Set delay_timer8 = Vx
  void Op_Fx18();  // 29) Set sound_timer8 = Vx
  void Op_Fx1E();  // 30) Set index16 = Vx + index16
  void Op_Fx29();  // 31) Set index16 = address(sprite => corresp. 0x num in Vx)
  void Op_Fx33();  // 32) Store Binary Coded Decimal equivalent of value in Vx
                   // @addresses `index16`, (`index16` + 1), (`index16` + 2)
  void Op_Fx55();  // 33) Store values of [V0 to Vx] in memory starting @addr.
                   // `index16`; Set `index16` = `index16` + x + 1 after storing
  void Op_Fx65();  // 34) Fill [V0 to Vx] with values in memory starting @addr.
                   // `index16`; Set `index16` = `index16` + x + 1 after filling

  std::mt19937 RandomGen;                              // Random engine
  std::uniform_int_distribution<uint8_t> random_byte;  // For a range of randnum
};

#endif  // CHIP8_CHIP8_H