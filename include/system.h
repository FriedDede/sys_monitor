#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  std::vector<std::string> procfileread(std::string);
  Processor& Cpu();                   // TODO: See src/system.cpp
  int TotalProcesses(); 
  std::vector<Process>& Processes();  
  float MemoryUtilization();          
  float Cpumean1m();
  long UpTime();                      
  int RunningProcesses();             // TODO: See src/system.cpp
  std::string Kernel();               // TODO: See src/system.cpp
  std::string OperatingSystem();      // TODO: See src/system.cpp
  std::string Hostname();
  std::vector<Process> processes_;
 private:
  Processor cpu_ ;
  
};

#endif
