#include "Net.h"

Net::Net()
{
    char str[255];
    dev = fopen(NET_DEV_PATH, "r");
    gettimeofday(&previous_time, NULL);
    while (!feof(dev))
    {
        fgets(str, 255, dev);
        if (strstr(str, "enp"))
        {
            ethernet = true;
            sscanf(str, "%*s %lld %*d %*d %*d %*d %*d %*d %*d %lld %*d %*d %*d %*d %*d %*d %*d",
                   &previous_received_ethernet, &previous_transmited_ethernet);
        }
        if (strstr(str, "wlp"))
        {
            wireless = true;
            sscanf(str, "%*s %lld %*d %*d %*d %*d %*d %*d %*d %lld %*d %*d %*d %*d %*d %*d %*d",
                   &previous_received_wireless, &previous_transmited_wireless);
        }
    }
}

double Net::get_receiving_speed(enum interface interface)
{
    switch (interface)
    {
    case WIRELESS:
        if (wireless)
            return receiving_speed_wireless;
    case ETHERNET:
        if (ethernet)
            return receiving_speed_ethernet;
    }
    return 0;
}

double Net::get_transmiting_speed(enum interface interface)
{
    switch (interface)
    {
    case WIRELESS:
        if (wireless)
            return transmiting_speed_wireless;
    case ETHERNET:
        if (ethernet)
            return transmiting_speed_ethernet;
    }
    return 0;
}

bool Net::get_interface(enum interface interface)
{
    switch (interface)
    {
    case WIRELESS:
        return wireless;
    case ETHERNET:
        return ethernet;
    }
    return false;
}

void Net::update_file()
{
    char str[255];
    while (!feof(dev))
        fgets(str, 255, dev);
}

void Net::update()
{
    struct timeval current_time;
    update_file();
    gettimeofday(&current_time, NULL);
    fseek(dev, 0, SEEK_SET);
    char str[255];
    long long received_bytes, transmited_bytes;
    bool flag_ethernet = true, flag_wireless = true;
    while (!feof(dev))
    {
        fgets(str, 255, dev);
        if (strstr(str, "enp") && flag_ethernet)
        {
            flag_ethernet = false;
            sscanf(str, "%*s %lld %*d %*d %*d %*d %*d %*d %*d %lld %*d %*d %*d %*d %*d %*d %*d",
                   &received_bytes, &transmited_bytes);
            receiving_speed_ethernet = (float)(received_bytes - previous_received_ethernet) /
                                       (current_time.tv_sec - previous_time.tv_sec +
                                        (float)(current_time.tv_usec - previous_time.tv_usec) / 1000000.0);
            receiving_speed_ethernet /= 1048576.0;
            receiving_speed_ethernet *= 8;
            transmiting_speed_ethernet = (float)(transmited_bytes - previous_transmited_ethernet) /
                                         (current_time.tv_sec - previous_time.tv_sec +
                                          (float)(current_time.tv_usec - previous_time.tv_usec) / 1000000.0);
            transmiting_speed_ethernet /= 1048576.0;
            transmiting_speed_ethernet *= 8;
            previous_received_ethernet = received_bytes;
            previous_transmited_ethernet = transmited_bytes;
        }
        if (strstr(str, "wlp") && flag_wireless)
        {
            flag_wireless = false;
            sscanf(str, "%*s %lld %*d %*d %*d %*d %*d %*d %*d %lld %*d %*d %*d %*d %*d %*d %*d",
                   &received_bytes, &transmited_bytes);
            receiving_speed_wireless = (float)(received_bytes - previous_received_wireless) /
                                       (current_time.tv_sec - previous_time.tv_sec +
                                        (float)(current_time.tv_usec - previous_time.tv_usec) / 1000000.0);
            receiving_speed_wireless /= 1048576.0;
            receiving_speed_wireless *= 8;
            transmiting_speed_wireless = (float)(transmited_bytes - previous_transmited_wireless) /
                                         (current_time.tv_sec - previous_time.tv_sec +
                                          (float)(current_time.tv_usec - previous_time.tv_usec) / 1000000.0);
            transmiting_speed_wireless /= 1048576.0;
            transmiting_speed_wireless *= 8;
            previous_received_wireless = received_bytes;
            previous_transmited_wireless = transmited_bytes;
        }
    }
    previous_time.tv_sec = current_time.tv_sec;
    previous_time.tv_usec = current_time.tv_usec;
}

Net::~Net()
{
    fclose(dev);
}