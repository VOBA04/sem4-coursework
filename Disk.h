#pragma once

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <vector>

const char BLOCK_PATH[] = "/sys/block/";
const char MODEL_PATH[] = "/device/model";
const char SECTOR_SIZE_PATH[] = "/queue/hw_sector_size";
const char DISK_STATS_PATH[] = "/proc/diskstats";

class Disk
{
    std::vector<std::string> devices;
    std::vector<std::string> models;
    std::vector<int> sector_size;
    int disks_count = 0;
    std::vector<long long> previous_sectors_readed, previous_sectors_writed;
    struct timeval previous_time;
    std::vector<double> read_speed;
    std::vector<double> write_speed;
    FILE *stats;

    void update_file();

public:
    Disk();
    std::string get_model(int);
    int get_disks_count();
    double get_read_speed(int);
    double get_write_speed(int);
    void update();
    ~Disk();
};