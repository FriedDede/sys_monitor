#include "processor.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include <unistd.h>
#include <vector>

//TODO: rewrite this function as partial_usage[process] sum.

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

// TODO: Return the aggregate CPU utilization
// TODO: check sta cagata
float Processor::Utilization() {  
    
    size_t previous_idle_time=0, previous_total_time=0;
    for (size_t idle_time, total_time; get_cpu_times(idle_time, total_time);) {
        const float idle_time_delta = idle_time - previous_idle_time;
        const float total_time_delta = total_time - previous_total_time;
        const float utilization = (1.0 - idle_time_delta / total_time_delta);
        return utilization;
        previous_idle_time = idle_time;
        previous_total_time = total_time;
    }
    /*
    size_t previous_idle_time=0, previous_total_time=0;
    size_t idle_time=0,total_time=0;
    get_cpu_times(idle_time,total_time);
    previous_idle_time=idle_time;
    previous_idle_time=total_time;
    get_cpu_times(idle_time,total_time);
    float utilization = (1.0 - (idle_time-previous_idle_time)/(total_time-previous_total_time));
    return utilization;*/
}

