#pragma once

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <unistd.h>

const char CPU_STAT_FILE_PATH[] = "/proc/stat";
const char CPU_INFO_FILE_PATH[] = "/proc/cpuinfo";
const char CPU_FREQ_DIR_PATH[] = "/sys/devices/system/cpu/cpufreq/policy0/";

class Cpu
{
    std::string model;
    int min_freq, max_freq;
    int processors_number;
    float usage;
    int avg_freq;
    std::vector<int> processors_freq;
    std::vector<float> processors_usage;
    std::vector<long long> prev_sum;
    std::vector<long long> prev_idle;
    FILE *stat;
    FILE *cpuinfo;

    void update_usage();
    void update_frequency();

public:
    Cpu();
    int get_processors_count();
    std::string get_model();
    int get_min_freq();
    int get_max_freq();
    float get_usage();
    float get_usage(int);
    std::vector<float> get_processors_usage();
    int get_freq();
    int get_freq(int);
    std::vector<int> get_processors_freq();
    void update();
    ~Cpu();
};