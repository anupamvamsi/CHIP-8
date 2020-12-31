#include "Chip8.h"

#include <chrono>
#include <cmath>
#include <fstream>
#include <random>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;  // 16 chars (0 to F), 5 Bytes each
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

// Modify default constructor
Chip8::Chip8()
    : RandomGen(std::chrono::system_clock::now().time_since_epoch().count())
// Ctor membr initializtn list (direct initializtn)

{
  // Initialize pc
  pc16 = START_ADDRESS;

  // Load fonts into mem
  for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
    memory8_4kb[FONTSET_START_ADDRESS + i] = fontset[i];
  }

  // Give random num b/w 0 and 255
  random_byte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

void Chip8::LoadRom(char const* filename) {
  // Open ROM as stream of bin, move file ptr to the end
  std::ifstream rom(filename, std::ios::binary | std::ios::ate);

  if (rom.is_open()) {
    // Get ROM size -> allocate buffer to hold contents
    std::streampos size = rom.tellg();
    char* buffer = new char[size];

    // Go to beginning of the ROM -> fill the buffer by reading
    rom.seekg(0, std::ios::beg);
    rom.read(buffer, size);
    rom.close();

    // Load ROM contents into CHIP-8's memory (start @0x200):
    for (long i = 0; i < size; ++i) {
      memory8_4kb[START_ADDRESS + i] = buffer[i];
    }

    delete[] buffer;
  }
}

// *********************************************************************** //
//                    THIRTY-FOUR INSTRUCTIONS OF CHIP-8                   //
// *********************************************************************** //

void Chip8::Op_00E0() {  // 01) CLS

  // Set entire video buffer to ZERO (black).
  memset(video32_64_32, 0, sizeof(video32_64_32));
}

void Chip8::Op_00EE() {  // 02) RET

  // Decrement sp -> pc = address in stack
  --sp8;
  pc16 = stack16_16[sp8];
}

void Chip8::Op_1nnn() {  // 03) JMP

  // opcode = 1nnn -> opcode AND 0fff ->  get `address` (nnn) -> pc = nnn
  uint16_t address = opcode16 & 0x0FFFu;

  pc16 = address;
}

void Chip8::Op_2nnn() {  // 04) CALL

  // store the addr pc is pointing to in stack -> ++sp ->
  // extract call addr from opcode (Op_1nnn) -> store in pc.
  stack16_16[sp8] = pc16;
  ++sp8;

  uint16_t address = opcode16 & 0x0FFFu;
  pc16 = address;
}

void Chip8::Op_3xnn() {  // 05) Skip next instruction if Vx = nn

  uint8_t extract_nn = opcode16 & 0x00FFu;
  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;

  if (registers8_16[v_x] == extract_nn) {
    pc16 += 2;
  }
}

void Chip8::Op_4xnn() {  // 06) Skip next instruction if Vx != nn

  uint8_t extract_nn = opcode16 & 0x00FFu;
  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;

  if (registers8_16[v_x] != extract_nn) {
    pc16 += 2;
  }
}

void Chip8::Op_5xy0() {  // 07) Skip next instruction if Vx = Vy

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x0F00u) >> 4u;

  if (registers8_16[v_x] == registers8_16[v_y]) {
    pc16 += 2;
  }
}

void Chip8::Op_6xnn() {  // 08) Set Vx = nn

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t extract_nn = opcode16 & 0x00FF;

  registers8_16[v_x] = extract_nn;
}

void Chip8::Op_7xnn() {  // 09) Set Vx = Vx + nn

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t extract_nn = opcode16 & 0x00FFu;

  registers8_16[v_x] += extract_nn;
}

void Chip8::Op_8xy0() {  // 10) Set Vx = Vy

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  registers8_16[v_x] = registers8_16[v_y];
}

void Chip8::Op_8xy1() {  // 11) Set Vx = Vx OR Vy

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  registers8_16[v_x] |= registers8_16[v_y];
}

void Chip8::Op_8xy2() {  // 12) Set Vx = Vx AND Vy

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  registers8_16[v_x] &= registers8_16[v_y];
}

void Chip8::Op_8xy3() {  // 13) Set Vx = Vx XOR Vy

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  registers8_16[v_x] ^= registers8_16[v_y];
}

void Chip8::Op_8xy4() {  // 14) Set Vx = Vx + Vy | Vf = 01 if Carry, else
                         // Vf = 00

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  uint16_t sum = registers8_16[v_x] + registers8_16[v_y];

  if (sum > 255U) {
    registers8_16[0xF] = 1;
  } else {
    registers8_16[0xF] = 0;
  }

  registers8_16[v_x] = sum & 0xFFu;  // to store only 8-bits of sum's data
}

void Chip8::Op_8xy5() {  // 15) Set Vx = Vx - Vy | Vf = 00 if borrow, else
                         // Vf = 01

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  if (registers8_16[v_x] > registers8_16[v_y]) {
    registers8_16[0xF] = 1;
  } else {
    registers8_16[0xF] = 0;
  }

  registers8_16[v_x] -= registers8_16[v_y];
}

void Chip8::Op_8xy6() {  // 16) Vy >> Vx | Shift Right Vy (1-bit) and store in
                         // Vx | Don't modify Vy | Store Vy's LSB in Vf before
                         // shifting | This differs from Cowgod & Austin Morlan

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  // Store Vy's LSB in Vf
  registers8_16[0xF] = registers8_16[v_y] & 0x1;

  registers8_16[v_x] = registers8_16[v_y] >> 1u;
}

void Chip8::Op_8xy7() {  // 17) Set Vx = Vy - Vx | Vf = 00 if borrow, else
                         // Vf = 01

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  if (registers8_16[v_y] > registers8_16[v_x]) {
    registers8_16[0xF] = 1;
  } else {
    registers8_16[0xF] = 0;
  }

  registers8_16[v_x] = registers8_16[v_y] - registers8_16[v_x];
}

void Chip8::Op_8xyE() {  // 18) Vx << Vy | Shift Left Vy (1-bit) -> store in Vx
                         // Don't modify Vy | Store Vy's MSB in Vf before
                         // shifting | This differs from Cowgod & Austin Morlan

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  // Store MSB in Vf
  registers8_16[0xF] = (registers8_16[v_y] & 0x80u) >> 7u;

  registers8_16[v_x] = registers8_16[v_y] << 1u;
}

void Chip8::Op_9xy0() {  // 19) Skip next instruction if Vx != Vy

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;

  if (registers8_16[v_x] != registers8_16[v_y]) {
    pc16 += 2;
  }
}

void Chip8::Op_Annn() {  // 20) Set index16 = nnn

  uint16_t extract_nnn = (opcode16 & 0x0FFFu);

  index16 = extract_nnn;
}

void Chip8::Op_Bnnn() {  // 21) Jump to address (nnn + V0)

  uint16_t extract_nnn = (opcode16 & 0x0FFFu);

  pc16 = extract_nnn + registers8_16[0];
}

void Chip8::Op_Cxnn() {  // 22) Set Vx = random number with mask of nn

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t extract_nn = opcode16 & 0x00FFu;

  // Vx = RandomNum AND nn
  registers8_16[v_x] = random_byte(RandomGen) & extract_nn;
}

void Chip8::Op_Dxyn() {  // 23) Draw sprite at (Vx, Vy) with n Bytes of sprite
                         // data starting at the address stored in `index16` |
                         // Set Vf = 01 if any set pixels are unset | Vf = 00
                         // otherwise

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t v_y = (opcode16 & 0x00F0u) >> 4u;
  uint8_t height_n = (opcode16 & 0x000Fu);  // height of sprite = n pixels

  // Wrap (cut off) if sprite goes beyond screen bounds
  uint8_t xPos = registers8_16[v_x] % VIDEO_WIDTH;
  uint8_t yPos = registers8_16[v_y] % VIDEO_HEIGHT;

  // Set Vf = 0
  registers8_16[0xF] = 0;

  for (unsigned int row = 0; row < height_n; ++row) {  // For `n` pixel height

    // Each graphic will be `n` pixels high, and `n` is stored in `height_n`
    uint8_t sprite_byte = memory8_4kb[index16 + row];  // sprite_byte = spB

    for (unsigned int col = 0; col < 8; ++col) {  // For each pxl (bit) in spB

      // Each graphic will be 8 pixels wide, that is why '8' is chosen

      // Extracting one sprite pixel (bit) from spB and determining if it is on
      // or not, and then determining if display pixel is on or not, and whether
      // to switch it on or off depending on the sprite pixel
      uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
      uint32_t* display_pixel =
          &video32_64_32[((yPos + row) * VIDEO_WIDTH) + (xPos + col)];

      // sp is on (sp = 1)
      if (sprite_pixel) {
        // dp is also on (dp = 1), set Vf = 01
        if (*display_pixel == 0xFFFFFFFF)  // it's uint32
        {
          registers8_16[0xF] = 1;
        }

        // sp XOR dp (only done for sp = 1 case, since not required for sp = 0)
        // dp ^= 0xFFFFFFFF because dp is 32 bits, and sp should also match that
        *display_pixel ^= 0xFFFFFFFF;
      }
    }
  }
}

void Chip8::Op_Ex9E() {  // 24) Skip next instruction if key with value in Vx is
                         // pressed

  uint8_t v_x = (opcode16 & 0x0F00) >> 8u;

  uint8_t key = registers8_16[v_x];

  if (keypad8_16[key]) {
    pc16 += 2;
  }
}

void Chip8::Op_ExA1() {  // 25) Skip next instruction if key with value in Vx is
                         // ~pressed

  uint8_t v_x = (opcode16 & 0x0F00) >> 8u;

  uint8_t key = registers8_16[v_x];

  if (!keypad8_16[key]) {
    pc16 += 2;
  }
}

void Chip8::Op_Fx07() {  // 26) Set Vx = delay_timer8

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;

  registers8_16[v_x] = delay_timer8;
}

void Chip8::Op_Fx0A() {  // 27) Wait for key press, store key value in Vx

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;

  if (keypad8_16[0])
    registers8_16[v_x] = 0;

  else if (keypad8_16[1])
    registers8_16[v_x] = 1;

  else if (keypad8_16[2])
    registers8_16[v_x] = 2;

  else if (keypad8_16[3])
    registers8_16[v_x] = 3;

  else if (keypad8_16[4])
    registers8_16[v_x] = 4;

  else if (keypad8_16[5])
    registers8_16[v_x] = 5;

  else if (keypad8_16[6])
    registers8_16[v_x] = 6;

  else if (keypad8_16[7])
    registers8_16[v_x] = 7;

  else if (keypad8_16[8])
    registers8_16[v_x] = 8;

  else if (keypad8_16[9])
    registers8_16[v_x] = 9;

  else if (keypad8_16[10])
    registers8_16[v_x] = 10;

  else if (keypad8_16[11])
    registers8_16[v_x] = 11;

  else if (keypad8_16[12])
    registers8_16[v_x] = 12;

  else if (keypad8_16[13])
    registers8_16[v_x] = 13;

  else if (keypad8_16[14])
    registers8_16[v_x] = 14;

  else if (keypad8_16[15])
    registers8_16[v_x] = 15;

  else
    pc16 -= 2;
}

void Chip8::Op_Fx15() {  // 28) Set delay_timer8 = Vx

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;

  delay_timer8 = registers8_16[v_x];
}

void Chip8::Op_Fx18() {  // 29) Set sound_timer8 = Vx

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;

  sound_timer8 = registers8_16[v_x];
}

void Chip8::Op_Fx1E() {  // 30) Set index16 = Vx + index16

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;

  index16 += registers8_16[v_x];
}

void Chip8::Op_Fx29() {  // 31) Set index16 = address of sprite => corresp. hex
                         // digit in Vx

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t hex_digit_in_vx = registers8_16[v_x];  // get hex digit from Vx

  index16 = FONTSET_START_ADDRESS + (hex_digit_in_vx * 5);  // each digit is 5B
}

void Chip8::Op_Fx33() {  // 32) Store Binary Coded Decimal equivalent of value
                         // in Vx @addresses `index16`, (`index16` + 1),
                         // (`index16` + 2)

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;
  uint8_t value_vx = registers8_16[v_x];

  for (int i = 0, j = 2; i < 3; i++, j--) {
    // Store 100s place in (idx), 10s place in (idx + 1), 1s place in (idx + 2)
    memory8_4kb[index16 + j] = (value_vx / (int)std::pow(10, i)) % 10;
  }
}

void Chip8::Op_Fx55() {  // 33) Store values of [V0 to Vx] in memory starting
                         // @address `index16` | Set `index16` = `index16` + x +
                         // 1 after storing | This differs from Cowgod & Austin

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;

  for (uint8_t i = 0; i < v_x + 1; i++) {
    memory8_4kb[index16 + i] = registers8_16[i];
  }

  index16 += v_x + 1;
}

void Chip8::Op_Fx65() {  // 34) Fill [V0 to Vx] with values in memory starting
                         // @address `index16` | Set `index16` = `index16` + x +
                         // 1 after filling | This differs from Cowgod & Austin

  uint8_t v_x = (opcode16 & 0x0F00u) >> 8u;

  for (uint8_t i = 0; i < v_x + 1; i++) {
    registers8_16[i] = memory8_4kb[index16 + i];
  }

  index16 += v_x + 1;
}

// ************************************************************************ //