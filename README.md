# CHIP-8
## Implementation(s) (yet another (and some more?)) of the CHIP-8 interpreter!

Just a project started out by aiming towards getting a hold on/learning C++. 

Resources for the project:

1. Primarily guided by: https://austinmorlan.com/posts/chip8_emulator/
2. However, as Point 1 claims, there is another reference that Point 1 follows - http://mattmik.com/files/chip8/mastering/chip8.html
3. An extension of Point 2 can be found at - https://github.com/mattmikolay/chip-8/wiki
4. The argument for using the reference in Points 2 and 3 _(and not the Cowgod reference that is more easily found)_ is presented well through the comments (by RodgerTheGreat) in this Reddit post by Bisqwit: https://www.reddit.com/r/programming/comments/3ca4ry/writing_a_chip8_interpreteremulator_in_c14_10/csu7w8k/

More resources can be found by going through the above four in detail (quite a rabbit hole for people interested in emulation of older machines).

## To build the interpreter:

1. Clone the repository.
2. Get the SDL Library - check out this guide for Visual Studio on how to set it up: https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php
3. Build the solution (assuming you are using Visual Studio: Ctrl + B). **DO NOT BUILD/INCLUDE `test_manual.cpp`**.
4. Go to bin > x64 > Debug through the command line (Windows cmd): `cd yourDirectoryPath\Chip8\bin\x64\Debug`
5. Once you are in the correct directory with the built .exe file, make sure you have the roms you need in it.
6. Through the command prompt (cmd), type: `Chip8.exe 10 3 test_opcode.ch8` `[Usage: Chip8.exe <Scale> <Delay> <ROM>]`
