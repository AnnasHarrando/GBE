#include "SDL.h"


void ui_init();
bool ui_handle_events();
void update_screen(uint32_t *buffer);
void update_dbg_window();
void t_update_dbg_window();
void oam_update_dbg_window(uint8_t *val);
void set_button_type(uint8_t val);
uint8_t get_button_press();
uint32_t get_fps();