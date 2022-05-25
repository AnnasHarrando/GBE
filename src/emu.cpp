#include "emu.h"

using namespace std;

static cpu cpu;
static cart cart;
static ram ram;
static io io;
static ppu ppu;
static bus bus;
static timer timer;
static lcd lcd;
static ppu_fifo ppu_fifo;

void save_states();
void load_states();
void init_objects();
DWORD WINAPI cpu_run(LPVOID lpParameter);


int start(char **argv) {
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

    while(ui_handle_events()){
        update_screen(ppu.buffer);
#if false
        update_dbg_window();
        t_update_dbg_window();
        oam_update_dbg_window(get_oam_tiles());
#endif

    }
}

void cycles(uint8_t cycle){
    for(int i=0; i<cycle; i++) {

        for (int j = 0; j < 4; j++) {

            timer.tick();
            ppu.tick();
        }

        ppu.dma_tick();
    }

    for(int i=0; i<0; i++){
        //timing
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

uint8_t bus_read(uint16_t addr){
    return bus.read(addr);
}

class cpu saved_cpu;
class cart saved_cart;
class ram saved_ram;
class io saved_io;
class ppu saved_ppu;
class bus saved_bus;
class timer saved_timer;
class lcd saved_lcd;
class ppu_fifo saved_ppu_fifo;


class cart *get_cart() { return &cart;}
class io *get_io() { return &io;}
class ram *get_ram() { return &ram;}
class ppu *get_ppu() { return &ppu;}
class cpu *get_cpu() { return &cpu;}
class bus *get_bus() { return &bus;}
class timer *get_timer() { return &timer;}
class lcd *get_lcd() { return &lcd;}
class ppu_fifo *get_ppu_fifo() { return &ppu_fifo;}

void save_states(){
    saved_bus = bus;
    saved_cart = cart;
    saved_cpu = cpu;
    saved_io = io;
    saved_lcd = lcd;
    saved_ppu = ppu;
    saved_ppu_fifo = ppu_fifo;
    saved_ram = ram;
    saved_timer = timer;
}

void load_states(){
    bus = saved_bus;
    cart = saved_cart;
    cpu = saved_cpu;
    io = saved_io;
    lcd = saved_lcd;
    ppu = saved_ppu;
    ppu_fifo = saved_ppu_fifo;
    ram = saved_ram;
    timer = saved_timer;
}

void init_objects(){
    inst_init();
    fifo_init();
    audio_init();
    bus_init();
    cpu_init();
    io_init();
    ppu_init();
    timer_init();
    ui_init();
}

uint8_t *get_oam_tiles(){
    uint8_t *temp = static_cast<uint8_t *>(malloc(40));
    for(int i=0; i<40; i++){
        temp[i] = ppu.oam_ram[i].tile;
    }
    return temp;
}