#include <cstdio>
#include "screen.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Surface *surface;

void ui_init(){
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");

    SDL_CreateWindowAndRenderer(1024,768,0,&window,&renderer);
}
bool ui_handle_events(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT) {

            SDL_DestroyWindow(window);
            return false;
        }
    }
    return true;
};