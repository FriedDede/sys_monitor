#pragma once
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  std::vector<std::string> Sysfileread(std::string);
  Processor& Cpu();                   
  int TotalProcesses(); 
  std::vector<Process>& Processes();  
  float MemoryUtilization();          
  float Cpumean1m();
  long UpTime();                      
  int RunningProcesses();             
  std::string Kernel();               
  std::string OperatingSystem();      
  std::string Hostname();
  std::vector<Process> processes_;
  
 private:
  Processor cpu_ ;
  
};
