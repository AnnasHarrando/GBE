#include <cstdio>
#include "audio.h"
#include <queue>

SDL_AudioSpec wavSpec;
Uint32 wavLength;
Uint8 *wavBuffer;
SDL_AudioDeviceID deviceId;

std::queue<uint16_t> time;
bool playing = false;
unsigned long starting_time = 0;

uint8_t NR21 = 0;
uint8_t NR22 = 0;
uint8_t NR23 = 0;
uint8_t NR24 = 0;

uint8_t NR52 = 0;

#define DUTY_NR ((NR21 & 0b11000000) >> 6)

uint8_t waveform[4][8] = {
        {0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,1,1},
        {0,0,0,0,1,1,1,1},
        {1,1,1,1,1,1,0,0}
};

void play(uint8_t val){
    uint16_t temp = (64-val)*4;
    time.push(temp);


}

void control_audio(){
    //if(playing) printf("playing");

    if(!time.empty()) {
        if (!playing) {
            SDL_QueueAudio(deviceId, wavBuffer, wavLength);
            SDL_PauseAudioDevice(deviceId, 0);
            starting_time = SDL_GetTicks();
            playing = true;
        }

        unsigned long cur_time = SDL_GetTicks();
        if (cur_time - starting_time > time.front()) {
            SDL_ClearQueuedAudio(deviceId);
            time.pop();
            if (!time.empty()) {
                SDL_QueueAudio(deviceId, wavBuffer, wavLength);
                SDL_PauseAudioDevice(deviceId, 0);
                starting_time = SDL_GetTicks();
            }
            else playing = false;
        }
    }

}

void audio_init(){

    if(SDL_Init(SDL_INIT_AUDIO) != 0) exit(-23);
    SDL_LoadWAV("C:/Users/annas/Downloads/src_beep-02.wav", &wavSpec, &wavBuffer, &wavLength);
    deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);


}