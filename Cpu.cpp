#include "Cpu.h"

Cpu::Cpu()
{
    FILE *fp = fopen(CPU_STAT_FILE_PATH, "r");
    char p_name[255];
    processors_number = 0;
    do
    {
        fgets(p_name, 255, fp);
        processors_number++;
    } while (!strncmp(p_name, "cpu", 3));
    fclose(fp);
    processors_number -= 2;
    processors_usage.resize(processors_number);
    processors_freq.resize(processors_number);
    char str[255];
    char *pos;
    fp = fopen(CPU_INFO_FILE_PATH, "r");
    while (!feof(fp))
    {
        fgets(str, 255, fp);
        if ((pos = strstr(str, "model name")))
        {
            pos += 12;
            while (!(isalpha(*pos) || isdigit(*pos)))
                pos++;
            model = pos;
            model.pop_back();
            break;
        }
    }
    fclose(fp);
    pos = (char *)malloc(255);
    strcpy(pos, CPU_FREQ_DIR_PATH);
    strcat(pos, "cpuinfo_max_freq");
    fp = fopen(pos, "r");
    fscanf(fp, "%d", &max_freq);
    max_freq /= 1000;
    fclose(fp);
    strcpy(pos, CPU_FREQ_DIR_PATH);
    strcat(pos, "cpuinfo_min_freq");
    fp = fopen(pos, "r");
    fscanf(fp, "%d", &min_freq);
    min_freq /= 1000;
    fclose(fp);
    free(pos);
    stat = fopen(CPU_STAT_FILE_PATH, "r");
    cpuinfo = fopen(CPU_INFO_FILE_PATH, "r");
    prev_sum.resize(processors_number + 1);
    prev_idle.resize(processors_number + 1);
    long long tasks_metrics[10];
    fscanf(stat, "%*s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
           &(tasks_metrics[0]), &(tasks_metrics[1]), &(tasks_metrics[2]), &(tasks_metrics[3]), &(tasks_metrics[4]),
           &(tasks_metrics[5]), &(tasks_metrics[6]), &(tasks_metrics[7]), &(tasks_metrics[8]), &(tasks_metrics[9]));
    for (int i = 0; i < 9; i++)
        prev_sum[0] += tasks_metrics[i];
    prev_idle[0] = tasks_metrics[3];
    for (int i = 0; i < processors_number; i++)
    {
        fscanf(stat, "%*s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
               &(tasks_metrics[0]), &(tasks_metrics[1]), &(tasks_metrics[2]), &(tasks_metrics[3]), &(tasks_metrics[4]),
               &(tasks_metrics[5]), &(tasks_metrics[6]), &(tasks_metrics[7]), &(tasks_metrics[8]), &(tasks_metrics[9]));
        for (int j = 0; j < 9; j++)
            prev_sum[i + 1] += tasks_metrics[j];
        prev_idle[i + 1] = tasks_metrics[3];
    }
    sleep(1);
    while (!feof(stat))
        fgets(str, 255, stat);
}

std::string Cpu::get_model()
{
    return model;
}

int Cpu::get_max_freq()
{
    return max_freq;
}

int Cpu::get_min_freq()
{
    return min_freq;
}

int Cpu::get_processors_count()
{
    return processors_number;
}

float Cpu::get_usage()
{
    return usage;
}

float Cpu::get_usage(int number)
{
    return processors_usage[number];
}

std::vector<float> Cpu::get_processors_usage()
{
    return processors_usage;
}

int Cpu::get_freq()
{
    return avg_freq;
}

int Cpu::get_freq(int number)
{
    return processors_freq[number];
}

std::vector<int> Cpu::get_processors_freq()
{
    return processors_freq;
}

void Cpu::update_usage()
{
    char str[255];
    while (!feof(stat))
        (void)fgets(str, 255, stat);
    fseek(stat, 0, SEEK_SET);
    long long tasks_metrics[10];
    long long sum[processors_number + 1] = {0};
    long long idle[processors_number + 1];
    fscanf(stat, "%*s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
           &(tasks_metrics[0]), &(tasks_metrics[1]), &(tasks_metrics[2]), &(tasks_metrics[3]), &(tasks_metrics[4]),
           &(tasks_metrics[5]), &(tasks_metrics[6]), &(tasks_metrics[7]), &(tasks_metrics[8]), &(tasks_metrics[9]));
    for (int i = 0; i < 9; i++)
        sum[0] += tasks_metrics[i];
    idle[0] = tasks_metrics[3] + tasks_metrics[4];
    for (int i = 0; i < processors_number; i++)
    {
        fscanf(stat, "%*s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
               &(tasks_metrics[0]), &(tasks_metrics[1]), &(tasks_metrics[2]), &(tasks_metrics[3]), &(tasks_metrics[4]),
               &(tasks_metrics[5]), &(tasks_metrics[6]), &(tasks_metrics[7]), &(tasks_metrics[8]), &(tasks_metrics[9]));
        for (int j = 0; j < 9; j++)
            sum[i + 1] += tasks_metrics[j];
        idle[i + 1] = tasks_metrics[3] + tasks_metrics[4];
    }
    usage = 100 - (float)(idle[0] - prev_idle[0]) * 100.0 / (float)(sum[0] - prev_sum[0]);
    prev_idle[0] = idle[0];
    prev_sum[0] = sum[0];
    for (int i = 0; i < processors_number; i++)
    {
        processors_usage[i] = 100 - (float)(idle[i + 1] - prev_idle[i + 1]) * 100.0 / (float)(sum[i + 1] - prev_sum[i + 1]);
        prev_idle[i + 1] = idle[i + 1];
        prev_sum[i + 1] = sum[i + 1];
    }
}

void Cpu::update_frequency()
{
    fseek(cpuinfo, 0, SEEK_SET);
    char str[255];
    char *pos;
    int i = 0;
    int sum = 0;
    while (!feof(cpuinfo))
    {
        fgets(str, 255, cpuinfo);
        if ((pos = strstr(str, "cpu MHz")))
        {
            int freq = 0;
            i++;
            pos += 9;
            while (!isdigit(*pos))
                pos++;
            while (isdigit(*pos))
            {
                freq = freq * 10 + (*pos - '0');
                pos++;
            }
            processors_freq[i - 1] = freq;
            sum += freq;
        }
    }
    sum /= i;
    avg_freq = sum;
}

void Cpu::update()
{
    update_usage();
    update_frequency();
}

Cpu::~Cpu()
{
    fclose(stat);
    fclose(cpuinfo);
}