#include "Disk.h"

Disk::Disk()
{
    DIR *dp = opendir(BLOCK_PATH);
    struct dirent *dirent;
    devices.resize(0);
    while ((dirent = readdir(dp)) != NULL)
    {
        if ((strcmp(dirent->d_name, ".") == 0) || (strcmp(dirent->d_name, "..") == 0) || (strncmp(dirent->d_name, "loop", 4) == 0))
            continue;
        disks_count++;
        devices.push_back(dirent->d_name);
    }
    closedir(dp);
    models.resize(0);
    for (int i = 0; i < disks_count; i++)
    {
        char path[255] = {};
        char model[255];
        strcat(path, BLOCK_PATH);
        strcat(path, devices[i].c_str());
        strcat(path, MODEL_PATH);
        FILE *fp = fopen(path, "r");
        fgets(model, 255, fp);
        fclose(fp);
        models.push_back(model);
        int j = models[i].size();
        j--;
        while (models[i][j] == ' ' || models[i][j] == '\n')
            j--;
        models[i].resize(j + 1);
    }
    sector_size.resize(0);
    for (int i = 0; i < disks_count; i++)
    {
        char path[255] = {};
        int size;
        strcat(path, BLOCK_PATH);
        strcat(path, devices[i].c_str());
        strcat(path, SECTOR_SIZE_PATH);
        FILE *fp = fopen(path, "r");
        fscanf(fp, "%d", &size);
        fclose(fp);
        sector_size.push_back(size);
    }
    previous_sectors_readed.resize(disks_count);
    previous_sectors_writed.resize(disks_count);
    read_speed.resize(disks_count);
    write_speed.resize(disks_count);
    stats = fopen(DISK_STATS_PATH, "r");
    gettimeofday(&previous_time, NULL);
    char str[255];
    char *pos;
    int i = 0;
    while (!feof(stats))
    {
        fgets(str, 255, stats);
        if (i < disks_count && (pos = strstr(str, devices[i].c_str())) && *(pos + devices[i].length()) == ' ')
        {
            i++;
            sscanf(str, "%*d %*d %*s %*d %*d %lld %*d %*d %*d %lld",
                   &previous_sectors_readed[i - 1], &previous_sectors_writed[i - 1]);
            fseek(stats, 0, SEEK_SET);
        }
    }
}

std::string Disk::get_model(int number)
{
    return models[number];
}

int Disk::get_disks_count()
{
    return disks_count;
}

double Disk::get_read_speed(int number)
{
    return read_speed[number];
}

double Disk::get_write_speed(int number)
{
    return write_speed[number];
}

void Disk::update_file()
{
    char str[255];
    while (!feof(stats))
        fgets(str, 255, stats);
}

void Disk::update()
{
    struct timeval current_time;
    update_file();
    gettimeofday(&current_time, NULL);
    fseek(stats, 0, SEEK_SET);
    char str[255];
    char *pos;
    int i = 0;
    while (!feof(stats))
    {
        fgets(str, 255, stats);
        if (i < disks_count && (pos = strstr(str, devices[i].c_str())) && *(pos + devices[i].length()) == ' ')
        {
            long long current_sectors_readed, current_sectors_writed;
            sscanf(str, "%*d %*d %*s %*d %*d %lld %*d %*d %*d %lld", &current_sectors_readed, &current_sectors_writed);
            read_speed[i] = (float)(current_sectors_readed - previous_sectors_readed[i]) * sector_size[i] /
                            (current_time.tv_sec - previous_time.tv_sec +
                             (float)(current_time.tv_usec - previous_time.tv_usec) / 1000000.0);
            read_speed[i] /= 1048576.0;
            write_speed[i] = (float)(current_sectors_writed - previous_sectors_writed[i]) * sector_size[i] /
                             (current_time.tv_sec - previous_time.tv_sec +
                              (float)(current_time.tv_usec - previous_time.tv_usec) / 1000000.0);
            write_speed[i] /= 1048576.0;
            previous_sectors_readed[i] = current_sectors_readed;
            previous_sectors_writed[i] = current_sectors_writed;
            i++;
            fseek(stats, 0, SEEK_SET);
        }
        if (feof(stats) && i < disks_count)
        {
            fseek(stats, 0, SEEK_SET);
            i++;
        }
    }
    previous_time.tv_sec = current_time.tv_sec;
    previous_time.tv_usec = current_time.tv_usec;
}

Disk::~Disk()
{
    fclose(stats);
}