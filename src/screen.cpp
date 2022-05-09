#include <cstdio>
#include "screen.h"
#include "emu.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Surface *surface;


SDL_Window *sdlDebugWindow;
SDL_Renderer *sdlDebugRenderer;
SDL_Texture *sdlDebugTexture;
SDL_Surface *debugScreen;

SDL_Window *t_sdlDebugWindow;
SDL_Renderer *t_sdlDebugRenderer;
SDL_Texture *t_sdlDebugTexture;
SDL_Surface *t_debugScreen;

static int scale = 4;

void ui_init(){
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");

    SDL_CreateWindowAndRenderer(1024,768,0,&window,&renderer);

    surface = SDL_CreateRGBSurface(0, 1024, 768, 32,
                                  0x00FF0000,
                                  0x0000FF00,
                                  0x000000FF,
                                  0xFF000000);

    texture = SDL_CreateTexture(renderer,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                1024, 768);

    SDL_CreateWindowAndRenderer(16 * 8 * scale, 32 * 8 * scale, 0,
                                &sdlDebugWindow, &sdlDebugRenderer);

    debugScreen = SDL_CreateRGBSurface(0, (16 * 8 * scale) + (16 * scale),
                                       (32 * 8 * scale) + (64 * scale), 32,
                                       0x00FF0000,
                                       0x0000FF00,
                                       0x000000FF,
                                       0xFF000000);

    sdlDebugTexture = SDL_CreateTexture(sdlDebugRenderer,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        (16 * 8 * scale) + (16 * scale),
                                        (32 * 8 * scale) + (64 * scale));

    SDL_CreateWindowAndRenderer(32 * 8 * scale, 32 * 8 * scale, 0,
                                &t_sdlDebugWindow, &t_sdlDebugRenderer);

    t_debugScreen = SDL_CreateRGBSurface(0, (32 * 8 * scale) + (32 * scale),
                                       (32 * 8 * scale) + (64 * scale), 32,
                                       0x00FF0000,
                                       0x0000FF00,
                                       0x000000FF,
                                       0xFF000000);

    t_sdlDebugTexture = SDL_CreateTexture(t_sdlDebugRenderer,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        (32 * 8 * scale) + (32 * scale),
                                        (32 * 8 * scale) + (64 * scale));

    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    SDL_SetWindowPosition(sdlDebugWindow, x + 1024 + 10, y);
}

static unsigned long tile_colors[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void display_tile(SDL_Surface *screen, uint16_t startLocation, uint16_t tileNum, int x, int y) {
    SDL_Rect rc;

    for (int tileY=0; tileY<16; tileY += 2) {
        uint8_t b1 = bus_read(startLocation + (tileNum * 16) + tileY);
        uint8_t b2 = bus_read(startLocation + (tileNum * 16) + tileY + 1);

        for (int bit=7; bit >= 0; bit--) {
            uint8_t hi = !!(b1 & (1 << bit)) << 1;
            uint8_t lo = !!(b2 & (1 << bit));

            uint8_t color = hi | lo;

            rc.x = x + ((7 - bit) * scale);
            rc.y = y + (tileY / 2 * scale);
            rc.w = scale;
            rc.h = scale;

            SDL_FillRect(screen, &rc, tile_colors[color]);
        }
    }
}

void t_display_tile(SDL_Surface *screen, uint16_t startLocation, int x, int y) {
    SDL_Rect rc;

    for (int tileY=0; tileY<16; tileY += 2) {
        uint8_t b1 = bus_read(startLocation + tileY);
        uint8_t b2 = bus_read(startLocation + tileY + 1);

        for (int bit=7; bit >= 0; bit--) {
            uint8_t hi = !!(b1 & (1 << bit)) << 1;
            uint8_t lo = !!(b2 & (1 << bit));

            uint8_t color = hi | lo;

            rc.x = x + ((7 - bit) * (scale/2));
            rc.y = y + (tileY / 2 * (scale/2));
            rc.w = (scale/2);
            rc.h = (scale/2);

            SDL_FillRect(screen, &rc, tile_colors[color]);
        }
    }
}

void update_screen(uint32_t *buffer){
    SDL_Rect rc;
    rc.x = rc.y = 0;
    rc.w = rc.h = 2048;

    for (int line_num = 0; line_num < 144; line_num++) {
        for (int x = 0; x < 160; x++) {
            rc.x = x * scale;
            rc.y = line_num * scale;
            rc.w = scale;
            rc.h = scale;

            SDL_FillRect(surface, &rc, buffer[x + (line_num * 160)]);
        }
    }

    SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}


void t_update_dbg_window() {
    int xDraw = 0;
    int yDraw = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = t_debugScreen->w;
    rc.h = t_debugScreen->h;
    SDL_FillRect(t_debugScreen, &rc, 0xFF111111);


    //384 tiles, 24 x 16
    for (int y=0; y<64; y++) {
        for (int x=0; x<32; x++) {
            uint16_t tile = 0x8000 + bus_read(0x9800 + x + y*32)*16;
            t_display_tile(t_debugScreen, tile, xDraw + (x * (scale/2)), yDraw + (y * (scale/2)));
            xDraw += (8 * (scale/2));
        }

        yDraw += (8 * (scale/2));
        xDraw = 0;
    }

    SDL_UpdateTexture(t_sdlDebugTexture, NULL, t_debugScreen->pixels, t_debugScreen->pitch);
    SDL_RenderClear(t_sdlDebugRenderer);
    SDL_RenderCopy(t_sdlDebugRenderer, t_sdlDebugTexture, NULL, NULL);
    SDL_RenderPresent(t_sdlDebugRenderer);
}

void update_dbg_window() {
    int xDraw = 0;
    int yDraw = 0;
    int tileNum = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = debugScreen->w;
    rc.h = debugScreen->h;
    SDL_FillRect(debugScreen, &rc, 0xFF111111);

    uint16_t addr = 0x8000;

    //384 tiles, 24 x 16
    for (int y=0; y<24; y++) {
        for (int x=0; x<16; x++) {
            display_tile(debugScreen, addr, tileNum, xDraw + (x * scale), yDraw + (y * scale));
            xDraw += (8 * scale);
            tileNum++;
        }

        yDraw += (8 * scale);
        xDraw = 0;
    }

    SDL_UpdateTexture(sdlDebugTexture, NULL, debugScreen->pixels, debugScreen->pitch);
    SDL_RenderClear(sdlDebugRenderer);
    SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
    SDL_RenderPresent(sdlDebugRenderer);
}


bool ui_handle_events(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT) {

            SDL_DestroyWindow(window);
            SDL_DestroyWindow(sdlDebugWindow);
            return false;
        }
    }
    return true;
};