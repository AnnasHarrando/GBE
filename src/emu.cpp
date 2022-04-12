#include "emu.h"

int start(int argc, char **argv) {
    cart_load(argv[1]);
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");
    init();
    cpu_init();

    while(true){
        step();
        //SDL_Delay(200);
    }
}