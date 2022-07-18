#include "emu.h"
#include <chrono>
#include <iostream>

using namespace std;

static cpu cpu;
static cart cart;
static ram ram;
static ppu ppu;
static timer timer;
static lcd lcd;
static ppu_fifo ppu_fifo;
static audio audio;

void save_states();
void load_states();
void init_objects();
DWORD WINAPI cpu_run(LPVOID lpParameter);

SDL_AudioSpec audioSpec;
SDL_AudioDeviceID dev;
float *buffer = static_cast<float *>(malloc(1024 * sizeof(float)));

int start(char **argv) {
    SDL_Init(SDL_INIT_AUDIO);

    SDL_zero(audioSpec);
    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_F32SYS;
    audioSpec.channels = 2;
    audioSpec.samples = 1024;	// Adjust as needed
    audioSpec.callback = NULL;

    dev = SDL_OpenAudioDevice(NULL,0,&audioSpec, NULL,0);

    SDL_PauseAudioDevice(dev, 0);

    cart.load_rom(argv[1]);

    init_objects();

    HANDLE hThread = CreateThread(
            nullptr,    // Thread attributes
            0,       // Stack size (0 = use default)
            cpu_run, // Thread start address
            nullptr,    // Parameter to pass to the thread
            0,       // Creation flags
            nullptr);

    if (hThread == nullptr)
    {
        printf("Thread initialization failed.\n");
        return 1;
    }

    static long prev_frame_time = 0;
    static long fps = 16;

    while(ui_handle_events()){
        uint32_t end_time = SDL_GetTicks();

        if((end_time - prev_frame_time) > fps){
            update_screen(ppu.buffer);
            prev_frame_time = SDL_GetTicks();
        }

#if false
        update_dbg_window();
        t_update_dbg_window();
        oam_update_dbg_window();
#endif

    }

    if(cart.battery) cart.battery_save();
}

uint8_t sample_period = 0;
uint16_t fs_counter = 0;
uint16_t buffer_index = 0;

void cycles(uint8_t cycle){

    for(int i=0; i<cycle; i++) {
        for (int j = 0; j < 4; j++) {
             timer.tick();
             ppu.tick();
             audio.tick();

            if(fs_counter == 8192) {
                fs_counter = 0;
                audio.fs_tick();
            }
            else fs_counter++;
        }

        if(ppu.dma_active)ppu.dma_tick();


        if(sample_period == (uint8_t)(23 / get_scale())){
            float temp = 0;

            if(audio.sound_enabled()) {
                if (((audio.NR51 >> 4) & 1)) temp += audio.left_vol() * (float) audio.channel1.cur_val / 10;
                if (((audio.NR51 >> 5) & 1)) temp += audio.left_vol() * (float) audio.channel2.cur_val / 10;
                if (((audio.NR51 >> 6) & 1)) temp += audio.left_vol() * (float) audio.NR3_cur_val / 10;
                if (((audio.NR51 >> 7) & 1)) temp += audio.left_vol() * (float) audio.channel4.cur_val / 10;
            }
            buffer[buffer_index] = temp;
            buffer_index++;

            if(audio.sound_enabled()) {
                temp = 0;
                if (audio.NR51 & 1) temp += audio.right_vol() * (float) audio.channel1.cur_val / 10;
                if (((audio.NR51 >> 1) & 1)) temp += audio.right_vol() * (float) audio.channel2.cur_val / 10;
                if (((audio.NR51 >> 2) & 1)) temp += audio.right_vol() * (float) audio.NR3_cur_val / 10;
                if (((audio.NR51 >> 3) & 1)) temp += audio.left_vol() * (float) audio.channel4.cur_val / 10;
            }

            buffer[buffer_index] = temp;
            buffer_index++;

            sample_period = 0;
        }
        else {
            sample_period++;
        }

        if(buffer_index == 2048){
            SDL_QueueAudio(dev, buffer, 8192);
            buffer_index = 0;
        }
    }


}

DWORD WINAPI cpu_run(LPVOID lpParameter)
{

    while(true){
        if(get_running()) cpu.step();

        if(save_state()) {
            printf("saving\n");
            save_states();
        }
        if(load_state()){
            printf("loading\n");
            load_states();
        }
    }
}

class cpu saved_cpu;
class cart saved_cart;
class ram saved_ram;
class ppu saved_ppu;
class timer saved_timer;
class lcd saved_lcd;
class ppu_fifo saved_ppu_fifo;


class cart *get_cart() { return &cart;}
class ram *get_ram() { return &ram;}
class ppu *get_ppu() { return &ppu;}
class cpu *get_cpu() { return &cpu;}
class timer *get_timer() { return &timer;}
class lcd *get_lcd() { return &lcd;}
class ppu_fifo *get_ppu_fifo() { return &ppu_fifo;}
class audio *get_audio() { return &audio;}

void save_states(){
    saved_cart = cart;
    saved_cpu = cpu;
    saved_lcd = lcd;
    saved_ppu = ppu;
    saved_ppu_fifo = ppu_fifo;
    saved_ram = ram;
    saved_timer = timer;
}

void load_states(){
    cart = saved_cart;
    cpu = saved_cpu;
    lcd = saved_lcd;
    ppu = saved_ppu;
    ppu_fifo = saved_ppu_fifo;
    ram = saved_ram;
    timer = saved_timer;
}

void init_objects(){
    inst_init();
    fifo_init();
    bus_init();
    io_init();
    ppu_init();
    timer_init();
    ui_init();
}

SDL_AudioDeviceID get_dev(){
    return dev;
}
