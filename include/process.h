#pragma once
#define PROCESS_H

#include <string>
#include <vector>
/*Basic class for Process representation, It contains relevant attributes as shown below*/
class Process
{

public:
   void Update();
   void SetPid(int);

   std::string Read_Name();
   std::string Read_Parent();
   std::string Read_User();
   std::string Read_Command();
   std::string Read_Ram();
   std::string Read_Status();
   int Read_Pid();
   float Read_Cpu();
   long int Read_Uptime();
   void Log(int);

private:
   std::string name;
   int pid = 0;
   std::string ppid;
   std::string uid;
   float cpu = 0.0f;
   std::string ram;
   std::string status;
   std::string command;
   long int uptime = 0;

   float CpuUtilization();
   std::string ParentPid();
   long int UpTime();
   bool Exist();
   std::string Name();
   std::string User();
   std::string Command();
   std::string Ram();
   std::string Status();

   std::vector<std::string> procfileread(std::string);

   float prev_u_time = 0;
   float prev_s_time = 0;
   float prev_uptime = 0;
};
