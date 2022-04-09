#include "emu.h"
#include "SDL.h"
#include "cart.h"
#include "cpu.h"
#include <cstdio>

int start(int argc, char **argv) {
    cart_load(argv[1]);
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");

    cpu CPU = cpu();

    while(true){
        CPU.step();
        SDL_Delay(200);
    }
}