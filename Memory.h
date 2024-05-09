#pragma once

#include <string.h>
#include <stdio.h>
#include <ctype.h>

const char MEMORY_INFO_FILE_PATH[] = "/proc/meminfo";

class Memory
{
    float total;
    float free;
    float available;
    float cached;
    float swap_total;
    float swap_free;
    FILE *meminfo;

public:
    Memory();
    float get_total();
    float get_free();
    float get_available();
    float get_cached();
    float get_swap_total();
    float get_swap_free();
    void update();
    ~Memory();
};