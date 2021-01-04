#include <chrono>
#include <iostream>
#include <string>

#include "chip_8.h"
#include "platform.h"

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM> \n";
    std::exit(EXIT_FAILURE);
  }

  int video_scale = std::stoi(argv[1]);
  int cycle_delay = std::stoi(argv[2]);
  char const* rom_file_name = argv[3];

  Platform platform_obj("CHIP-8 INTERPRETER", VIDEO_WIDTH * video_scale,
                        VIDEO_HEIGHT * video_scale, VIDEO_WIDTH, VIDEO_HEIGHT);

  Chip8 chip8_obj;

  chip8_obj.LoadRom(rom_file_name);

  //std::cout << "ROM LOADED" << std::endl;

  int video_pitch = sizeof(chip8_obj.video32_64_32[0]) * VIDEO_WIDTH;

  auto last_cycle_time = std::chrono::high_resolution_clock::now();
  bool quit = false;

  while (!quit) {
    quit = platform_obj.ProcessInput(chip8_obj.keypad8_16);

    auto current_time = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(
                   current_time - last_cycle_time)
                   .count();

    if (dt > cycle_delay) {
      last_cycle_time = current_time;

      chip8_obj.Cycle();

      platform_obj.Update(chip8_obj.video32_64_32, video_pitch);
    }
  }

  return 0;
}