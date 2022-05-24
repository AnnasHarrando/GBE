#include <cstdio>
#include "screen.h"
#include "emu.h"
#include "lcd.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Surface *surface;

SDL_Window *sdlDebugWindow;
SDL_Renderer *sdlDebugRenderer;
SDL_Texture *sdlDebugTexture;
SDL_Surface *debugScreen;

SDL_Window *BgDebugWindow;
SDL_Renderer *BgDebugRenderer;
SDL_Texture *BgDebugTexture;
SDL_Surface *BgDebugScreen;

SDL_Window *OamDebugWindow;
SDL_Renderer *OamDebugRenderer;
SDL_Texture *OamDebugTexture;
SDL_Surface *OamDebugScreen;

SDL_GameController *gamecontroller = nullptr;

static int scale = 4;

void ui_init(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
    printf("SDL INIT\n");


    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)){
            gamecontroller = SDL_GameControllerOpen(i);
            break;
        }
    }

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
#if false
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
                                &BgDebugWindow, &BgDebugRenderer);

    BgDebugScreen = SDL_CreateRGBSurface(0, (32 * 8 * scale) + (32 * scale),
                                       (32 * 8 * scale) + (64 * scale), 32,
                                       0x00FF0000,
                                       0x0000FF00,
                                       0x000000FF,
                                       0xFF000000);

    BgDebugTexture = SDL_CreateTexture(BgDebugRenderer,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        (32 * 8 * scale) + (32 * scale),
                                        (32 * 8 * scale) + (64 * scale));

    SDL_CreateWindowAndRenderer(16 * 8 * scale, 16 * 8 * scale, 0,
                                &OamDebugWindow, &OamDebugRenderer);

    OamDebugScreen = SDL_CreateRGBSurface(0, (16 * 8 * scale) + (16 * scale),
                                         (16 * 8 * scale) + (16 * scale), 32,
                                         0x00FF0000,
                                         0x0000FF00,
                                         0x000000FF,
                                         0xFF000000);

    OamDebugTexture = SDL_CreateTexture(OamDebugRenderer,
                                          SDL_PIXELFORMAT_ARGB8888,
                                          SDL_TEXTUREACCESS_STREAMING,
                                          (16 * 8 * scale) + (16 * scale),
                                          (16 * 8 * scale) + (16 * scale));
#endif
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

void bg_display_tile(SDL_Surface *screen, uint16_t startLocation, int x, int y) {
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

void oam_display_tile(SDL_Surface *screen, uint16_t startLocation, int x, int y) {
    SDL_Rect rc;

    for (int tileY=0; tileY<16; tileY += 2) {
        uint8_t b1 = bus_read(startLocation + tileY);
        uint8_t b2 = bus_read(startLocation + tileY + 1);

        for (int bit=7; bit >= 0; bit--) {
            uint8_t hi = !!(b1 & (1 << bit)) << 1;
            uint8_t lo = !!(b2 & (1 << bit));

            uint8_t color = hi | lo;

            rc.x = x + ((7 - bit) * (scale));
            rc.y = y + (tileY / 2 * (scale));
            rc.w = (scale);
            rc.h = (scale);

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

bool bg_mode = true;

void t_update_dbg_window() {
    int xDraw = 0;
    int yDraw = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = BgDebugScreen->w;
    rc.h = BgDebugScreen->h;
    SDL_FillRect(BgDebugScreen, &rc, 0xFF111111);


    for (int y=0; y<64; y++) {
        for (int x=0; x<32; x++) {
            uint8_t index = bus_read(0x9800 + x + y*32);
            uint16_t tile;
            if(bg_mode) tile = 0x8000 + index*16;
            else{
                index += 128;
                tile = 0x8800 + index*16;
            }
            bg_display_tile(BgDebugScreen, tile, xDraw + (x * (scale/2)), yDraw + (y * (scale/2)));
            xDraw += (8 * (scale/2));
        }

        yDraw += (8 * (scale/2));
        xDraw = 0;
    }

    SDL_UpdateTexture(BgDebugTexture, NULL, BgDebugScreen->pixels, BgDebugScreen->pitch);
    SDL_RenderClear(BgDebugRenderer);
    SDL_RenderCopy(BgDebugRenderer, BgDebugTexture, NULL, NULL);
    SDL_RenderPresent(BgDebugRenderer);
}

void oam_update_dbg_window(uint8_t *val) {
    int xDraw = 0;
    int yDraw = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = OamDebugScreen->w;
    rc.h = OamDebugScreen->h;
    SDL_FillRect(OamDebugScreen, &rc, 0xFF111111);


    for (int y=0; y<4; y++) {
        for (int x=0; x<10; x++) {
            uint16_t tile = 0x8000 + val[x + y*10] *16;
            oam_display_tile(OamDebugScreen, tile, xDraw + (x * (scale)), yDraw + (y * (scale)));
            xDraw += (8 * (scale));
        }

        yDraw += (8 * (scale));
        xDraw = 0;
    }

    SDL_UpdateTexture(OamDebugTexture, NULL, OamDebugScreen->pixels, OamDebugScreen->pitch);
    SDL_RenderClear(OamDebugRenderer);
    SDL_RenderCopy(OamDebugRenderer, OamDebugTexture, NULL, NULL);
    SDL_RenderPresent(OamDebugRenderer);
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


bool dir_button;
bool action_button;
bool start_button;
bool select_button;
bool a;
bool b;
bool up;
bool down;
bool left;
bool right;

bool running = true;

uint32_t fps = 1000/60;

bool get_running(){
    return running;
}

uint32_t get_fps(){
    return fps;
}

void set_button_type(uint8_t val){
    dir_button = val & 0x20;
    action_button = val & 0x10;
}

uint8_t get_button_press(){
    uint8_t val = 0b11000000;

    if(action_button){
        if(!a) val |= 0b1;
        if(!b) val |= 0b10;
        if(!select_button) val |= 0b100;
        if(!start_button) val |= 0b1000;
    }
    if(dir_button){
        if(!right) val |= 0b1;
        if(!left) val |= 0b10;
        if(!up) val |= 0b100;
        if(!down) val |= 0b1000;
    }
    return val;
}

bool ui_handle_events(){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        if(event.type == SDL_KEYDOWN){
            switch(event.key.keysym.sym){
                case SDLK_w: up = true; break;
                case SDLK_s: down = true; break;
                case SDLK_a: left = true; break;
                case SDLK_d: right = true; break;
                case SDLK_l: a = true; break;
                case SDLK_k: b = true; break;
                case SDLK_p: start_button = true; printf("start"); break;
                case SDLK_u: select_button = true; break;
                case SDLK_1: fps = 1000/60; break;
                case SDLK_2: fps = 500/60; break;
                case SDLK_3: fps = 125/60; break;
                case SDLK_SPACE: running = !running; break;
                case SDLK_LALT: bg_mode = !bg_mode; break;
            }
        }
        if(event.type == SDL_KEYUP){
            switch(event.key.keysym.sym){
                case SDLK_w: up = false; break;
                case SDLK_s: down = false; break;
                case SDLK_a: left = false; break;
                case SDLK_d: right = false; break;
                case SDLK_l: a = false; break;
                case SDLK_k: b = false; break;
                case SDLK_p: start_button = false; break;
                case SDLK_u: select_button = false; break;
            }
        }

        if(gamecontroller != nullptr) {
            if (SDL_GameControllerGetButton(gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_UP)) up = true;
            else up = false;

            if (SDL_GameControllerGetButton(gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) down = true;
            else down = false;

            if (SDL_GameControllerGetButton(gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) left = true;
            else left = false;

            if (SDL_GameControllerGetButton(gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) right = true;
            else right = false;

            if (SDL_GameControllerGetButton(gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_UP)) up = true;
            else up = false;

            if (SDL_GameControllerGetButton(gamecontroller, SDL_CONTROLLER_BUTTON_B)) a = true;
            else a = false;

            if (SDL_GameControllerGetButton(gamecontroller, SDL_CONTROLLER_BUTTON_A)) b = true;
            else b = false;

            if (SDL_GameControllerGetButton(gamecontroller, SDL_CONTROLLER_BUTTON_GUIDE)) select_button = true;
            else select_button = false;

            if (SDL_GameControllerGetButton(gamecontroller, SDL_CONTROLLER_BUTTON_START)) start_button = true;
            else start_button = false;
        }

        if (event.type == SDL_QUIT) {

            SDL_DestroyWindow(window);
            SDL_DestroyWindow(sdlDebugWindow);
            return false;
        }
    }
    return true;
};