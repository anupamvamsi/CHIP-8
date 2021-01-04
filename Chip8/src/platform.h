#ifndef CHIP8_PLATFORM_H

#define CHIP8_PLATFORM_H

#include <cstdint>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class Platform {
 public:
  Platform(char const* title, int windowWidth, int windowHeight,
           int textureWidth, int textureHeight);
  ~Platform();
  void Update(void const* buffer, int pitch);
  bool ProcessInput(uint8_t* keys);

 private:
  SDL_Window* window{};
  SDL_Renderer* renderer{};
  SDL_Texture* texture{};
};

#endif  // CHIP8_PLATFORM_H
