#include "Memory.h"

Memory::Memory()
{
    FILE *fp = fopen(MEMORY_INFO_FILE_PATH, "r");
    char str[255];
    int memory = 0;
    int i = 0;
    do
    {
        fgets(str, 255, fp);
    } while (strncmp(str, "MemTotal:", 9));
    while (!isdigit(str[i]))
        i++;
    while (isdigit(str[i]))
    {
        memory = memory * 10 + (str[i] - '0');
        i++;
    }
    fclose(fp);
    total = memory / 1048576.0;
    meminfo = fopen(MEMORY_INFO_FILE_PATH, "r");
}

float Memory::get_total()
{
    return total;
}

float Memory::get_free()
{
    return free;
}

float Memory::get_available()
{
    return available;
}

float Memory::get_cached()
{
    return cached;
}

float Memory::get_swap_total()
{
    return swap_total;
}

float Memory::get_swap_free()
{
    return swap_free;
}

void Memory::update()
{
    fseek(meminfo, 0, SEEK_SET);
    char str[255];
    int memory;
    int i;
    while (!feof(meminfo))
    {
        fgets(str, 255, meminfo);
        i = 0;
        memory = 0;
        if (!strncmp(str, "MemFree:", 8))
        {
            while (!isdigit(str[i]))
                i++;
            while (isdigit(str[i]))
            {
                memory = memory * 10 + (str[i] - '0');
                i++;
            }
            free = memory / 1048576.0;
        }
        if (!strncmp(str, "MemAvailable:", 13))
        {
            while (!isdigit(str[i]))
                i++;
            while (isdigit(str[i]))
            {
                memory = memory * 10 + (str[i] - '0');
                i++;
            }
            available = memory / 1048576.0;
        }
        if (!strncmp(str, "Cached:", 7))
        {
            while (!isdigit(str[i]))
                i++;
            while (isdigit(str[i]))
            {
                memory = memory * 10 + (str[i] - '0');
                i++;
            }
            cached = memory / 1048576.0;
        }
        if (!strncmp(str, "SwapTotal:", 10))
        {
            while (!isdigit(str[i]))
                i++;
            while (isdigit(str[i]))
            {
                memory = memory * 10 + (str[i] - '0');
                i++;
            }
            swap_total = memory / 1048576.0;
        }
        if (!strncmp(str, "SwapFree:", 9))
        {
            while (!isdigit(str[i]))
                i++;
            while (isdigit(str[i]))
            {
                memory = memory * 10 + (str[i] - '0');
                i++;
            }
            swap_free = memory / 1048576.0;
        }
    }
}

Memory::~Memory()
{
    fclose(meminfo);
}