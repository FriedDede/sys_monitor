#pragma once
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
    System();
    std::vector<std::string> SysFileread(std::string);
    Processor&  Cpu();
    unsigned short         TotalProcesses(); 
    void Processes();
    float       MemoryUtilization();
    float       MemorySwap();
    float       MemoryShared();
    float       MemoryBuffer();
    
    long        UpTime();
    int         RunningProcesses();
    std::string Kernel();
    std::string OperatingSystem();
    std::string Hostname();
    std::vector<Process> processes_;
    
    float memory_Utilization = 0.0f;    
    float memory_Shared = 0.0f; 
    float memory_Swap = 0.0f;
    float memory_Buffer = 0.0f;
    unsigned short total_processes = 0;
    float cpu5m = 0.0f;
    float cpu1m = 0.0f;
    float cpu_usage = 0.0f;
    Processor cpu_ ;
 private:
    
};
