#include "processor.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include <unistd.h>
#include <vector>
#include <sys/sysinfo.h>

std::vector<size_t> get_cpu_times()
{
    std::ifstream proc_stat("/proc/stat");
    // Skip the 'cpu' prefix.
    proc_stat.ignore(5, ' ');
    std::vector<size_t> times;
    for (size_t time; proc_stat >> time; times.push_back(time))
        ;
    return times;
}

bool get_cpu_times(size_t &idle_time, size_t &total_time)
{
    const std::vector<size_t> cpu_times = get_cpu_times();
    if (cpu_times.size() < 4)
        return false;
    idle_time = cpu_times[3];
    total_time = std::accumulate(cpu_times.begin(), cpu_times.end(), 0);
    return true;
}

float Processor::Utilization()
{
    size_t idle_time = 0, total_time = 0;
    get_cpu_times(idle_time, total_time);
    const float idle_time_delta = idle_time - Processor::CPU_Previous_Idle;
    const float total_time_delta = total_time - Processor::CPU_Previous_Total;
    const float utilization = (1.0 - idle_time_delta / total_time_delta);
    Processor::CPU_Previous_Idle = idle_time;
    Processor::CPU_Previous_Total = total_time;

    for (int j = 0; j < 29; j++)
    {
        Processor::Cpu_Usage_Log[j] = Processor::Cpu_Usage_Log[j + 1];
    }
    Processor::Cpu_Usage_Log[29] = utilization * 100;
    return utilization;
}

int Processor::CoreCount()
{
    std::string Cpucount;
    std::vector<std::string> words;
    std::string path = "/proc/cpuinfo";
    std::ifstream proc_Sys(path.c_str(), std::ifstream::in);
    std::string str;
    while (proc_Sys >> str)
    {
        words.push_back(str);
    }
    proc_Sys.close();
    for (size_t i = 0; i < words.size(); i++)
    {
        if (words[i] == "siblings")
        {
            Cpucount = words[i + 2];
        }
    }
    Processor::Cpu_Count = atoi(Cpucount.c_str());
    return Processor::Cpu_Count;
}

float Processor::Cpumean1m()
{
    struct sysinfo info;
    sysinfo(&info);

    //output format must be 0.usage;
    //info.load range is 0 to 1000;
    float avgload = info.loads[0] / 100000.0;
    return avgload;
}
float Processor::Cpumean5m()
{
    struct sysinfo info;
    sysinfo(&info);

    //output format must be 0.usage;
    //info.load range is 0 to 1000;
    float avgload = info.loads[1] / 100000.0;
    return avgload;
}

Processor::Processor(void)
{
    for (auto &usage : Cpu_Usage_Log)
    {
        usage = 0;
    }
}