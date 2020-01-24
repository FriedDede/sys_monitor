#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
/*Basic class for Process representation, It contains relevant attributes as shown below*/
class Process {
 public:
  std::vector<std::string> procfileread(std::string);
  int Pid();        
  std::string Parent_Pid();                       
  void Pid_Insec(int);
  std::string Name();
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();
  std::string status();                       // TODO: See src/process.cpp
  bool exist();
  std::string status_buffer;
private:
  int process_ID;  
};
#endif