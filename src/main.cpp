#include "emu.h"

int main(int argv, char* args[])
{
    if(argv > 1) start(args);
    else printf("No file path to load in. \n");
    return 0;
}