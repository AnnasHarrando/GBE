#ifndef GBE_SCREEN_H
#define GBE_SCREEN_H


#include "SDL.h"


void ui_init();
bool ui_handle_events();
void update_screen(uint32_t *buffer);
void update_dbg_window();
void t_update_dbg_window();
void oam_update_dbg_window();
void set_button_type(uint8_t val);
uint8_t get_button_press();
double get_scale();

bool save_state();
bool load_state();
bool get_running();

#endif GBE_SCREEN_H
