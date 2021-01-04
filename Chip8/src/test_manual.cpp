#include <bitset>
#include <cstdint>
#include <iostream>

using namespace std;

int main() {

  ////////////////////////////////////
  // Bit extraction check

  uint16_t o = 0x3218u;
  uint8_t vx = (o & 0x0F00u) >> 8u;
  uint8_t bb = o & 0x00FFu;

  cout << std::hex << (unsigned)o << " " << (unsigned)vx << " " << (unsigned)bb
       << " ";

  ////////////////////////////////////
  // uniform initialization checker

  uint8_t registers8_16[16]{};

  cout << (unsigned)registers8_16[0];

  ///////////////////////////////////
  // Dxyn

  uint16_t spriteb = 0xF0;

  for (unsigned int col = 0; col < 8; ++col) {
    uint8_t spritePixel = spriteb & (0x80u >> col);
    cout << std::bitset<8>(spritePixel) << " " << std::bitset<8>(spriteb)
         << endl;
  }

  //////////////////////////////////
  // Dxyn

  cout << ((0x01 + 2) * 64) + (0x03 + 4) << endl;

  /////////////////////////////////
  // Fx33

  int arr[3];
  int index16 = 0;
  int value_vx = 352;

  /*
  for (int i = 0, j = 4; i < 5; i++, j--) {
    arr[index16 + j] = (value_vx / (int)std::pow(10, (i))) % 10;
  }
  */

  for (int i = 0, j = 2; i < 3; i++, j--) {
    arr[index16 + j] = (value_vx / (int)std::pow(10, i)) % 10;
  }

  for (int i = 0; i < 3; i++) {
    cout << "arr[" << i << "]" << arr[index16 + i] << "\n";
  }

  //////////////////////////////////
}