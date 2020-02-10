#include "processor.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include <unistd.h>
#include <vector>

std::vector<size_t> get_cpu_times() {
    std::ifstream proc_stat("/proc/stat");
    proc_stat.ignore(5, ' '); // Skip the 'cpu' prefix.
    std::vector<size_t> times;
    for (size_t time; proc_stat >> time; times.push_back(time));
    return times;
}

bool get_cpu_times(size_t &idle_time, size_t &total_time) {
    const std::vector<size_t> cpu_times = get_cpu_times();
    if (cpu_times.size() < 4)
        return false;
    idle_time = cpu_times[3];
    total_time = std::accumulate(cpu_times.begin(), cpu_times.end(), 0);
    return true;
}

float Processor::Utilization() {  
    size_t idle_time, total_time; get_cpu_times(idle_time, total_time);
    const float idle_time_delta = idle_time - Processor::CPU_Previous_Idle;
    const float total_time_delta = total_time - Processor::CPU_Previous_Total;
    const float utilization = (1.0 - idle_time_delta / total_time_delta);
    Processor::CPU_Previous_Idle = idle_time;
    Processor::CPU_Previous_Total = total_time;
    return utilization;
    
}

int Processor::Cpu() {
    return 0;
    //return Processor::Cpu_Count;
}



