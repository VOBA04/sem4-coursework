#pragma once

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

const char NET_DEV_PATH[] = "/proc/net/dev";

enum interface
{
    WIRELESS,
    ETHERNET
};

class Net
{
    long long previous_received_wireless, previous_transmited_wireless;
    long long previous_received_ethernet, previous_transmited_ethernet;
    struct timeval previous_time;
    bool wireless = false, ethernet = false;
    double receiving_speed_wireless = 0, transmiting_speed_wireless = 0;
    double receiving_speed_ethernet = 0, transmiting_speed_ethernet = 0;
    FILE *dev;

    void update_file();

public:
    Net();
    double get_receiving_speed(enum interface);
    double get_transmiting_speed(enum interface);
    bool get_interface(enum interface);
    void update();
    ~Net();
};