#pragma once
#define PROCESSOR_H

class Processor {
    
 public:
    Processor();
    float     Utilization();
    int       CoreCount();
    float     Cpumean1m();
    float     Cpumean5m();
    float     Cpu_Usage_Log[30];
  
 private:
    int     Cpu_Count;
    float   CPU_Previous_Idle = 0;
    float   CPU_Previous_Total = 0;
};
