#pragma once
#define PROCESS_H

#include <string>
#include <vector>
/*Basic class for Process representation, It contains relevant attributes as shown below*/
class Process {
 public:
 
  void        Update();
  std::vector<std::string> procfileread(std::string);  
  void        Pid_Insec(int);
  std::string Read_Name();
  std::string Read_Parent();
  std::string Read_User();
  std::string Read_Command();
  std::string Read_Ram();
  std::string Read_Status();
  int         Read_Pid();
  float       Read_Cpu();
  long int    Read_Uptime();
  void        Log(int);
private:

  std::string name;
  int         process_ID = 0;
  std::string pP_ID;
  std::string u_ID;
  float       cpu_Usage = 0;
  std::string ram_Usage;
  std::string status_buffer;
  std::string command;
  long int    uptime = 0;

  float       CpuUtilization();         
  std::string Parent_Pid();
  long int    UpTime();                       
  bool        exist();
  std::string Name();
  std::string User();
  std::string Command();
  std::string Ram();
  std::string Status();

  float       prev_u_time=0;
  float       prev_s_time=0;
  float       prev_uptime=0;
};
