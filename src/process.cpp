#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sys/sysinfo.h>
#include <thread>

#include "process.h"
#include "system.h"

using std::string;
using std::to_string;
using std::vector;

vector<string>  Process::procfileread(std::string filename){
       std::vector<std::string> words;
    if (Process::exist())
    {
        std::string str_pid = std::to_string(Process::process_ID);
        std::string path= "/proc/" + str_pid + "/" + filename;   
        std::ifstream proc_pid_status (path.c_str(), std::ifstream::in);
        std::string str;

        while (proc_pid_status >> str){                  
            words.push_back(str);
        }
        proc_pid_status.close();        
        return words;
    }
    else
    {
        std::string NotFoundFlag = "Not Found";
        words.push_back(NotFoundFlag);
        return words;
    }
}

string          Process::Name() {
    if (Process::exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        for (int i = 0; i < words.size(); i++){
            if (words[i]== "Name:"){
                return words[++i];
            }
        }
    }
    else{return "Not Found";} 
}

string          Process::Status(){
    if (Process::exist())
    {
        Process::status_buffer.clear();
        std::vector<std::string> words = Process::procfileread("status");
        for (int i = 0; i < words.size(); i++)
        {
            if (words[i]== "State:")
            {
                return words[++i];
            }
        }
    }
    else
    {
        return "Not Found";
    }  
}

std::string     Process::Parent_Pid(){
        if (Process::exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        for (int i = 0; i < words.size(); i++)
        {
            if (words[i]== "PPid:"){return words[++i];}
        }
    }
    else
    {
        return "Not Found";
    } 
}    

void            Process::Pid_Insec(int buff_pid){
    Process::process_ID=buff_pid;
}

// TODO: Return this process's CPU utilization
float           Process::CpuUtilization() {
    if (Process::exist())
    {
    float s_time, delta_s_time;
    float u_time, delta_u_time;
    float uptime, delta_uptime;

    vector<string> stat = procfileread("stat");
    u_time=(float)(atoi(stat[13].c_str()))/sysconf(_SC_CLK_TCK);
    s_time=(float)(atoi(stat[14].c_str()))/sysconf(_SC_CLK_TCK);
    uptime=(float)Process::UpTime();

    delta_s_time=s_time-Process::prev_s_time;
    delta_u_time=u_time-Process::prev_u_time;
    delta_uptime=uptime-Process::prev_uptime;

    Process::prev_uptime=uptime;
    Process::prev_u_time=u_time;
    Process::prev_s_time=s_time;

    return (delta_s_time+delta_u_time)/delta_uptime;
    }
    else
    {
        return 0;
    }
}

// Return the command that generates the process   
string          Process::Command() {
    if (Process::exist())
    {
        std::vector<std::string> words = Process::procfileread("comm");
        return words[0];
    }
    else
    {
        return "Not Found";
    }
    
}

// Return this process's memory utilization
string          Process::Ram() {
    if (Process::exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        
        for (int i = 0; i < words.size(); i++)
        {
            if (words[i]== "VmSize:")
            {
                return words[++i];
            }
        }
    }
    else
    {
        return "Not Found";
    }  
}

// Return the user (id) that generated this process
string          Process::User() {
        if (Process::exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        for (int i = 0; i < words.size(); i++)
        {
            if (words[i]== "Uid:")
            {
                return words[++i];
            }
        }
    }
    else
    {
        return "Not Found";
    } 
}

// TODO: Return the age of this process (in seconds)
long int        Process::UpTime() { 
    
    if (Process::exist())
    {
    long int time = 0;
    vector<string>stat=procfileread("stat");
    struct sysinfo info;
    sysinfo(&info);
    time=info.uptime-(atoi(stat[21].c_str())/sysconf(_SC_CLK_TCK));
    return time;
    }
    else
    {
        return 0;
    }
}

bool            Process::exist(){
    std::string str_pid = std::to_string(Process::process_ID);
    std::string path= "/proc/" + str_pid + "/status";   ;
    std::ifstream proc_pid_status (path.c_str(), std::ifstream::in);
    bool file_status= (bool)proc_pid_status;
    return file_status;
}

void            Process::Update(){
    if (Process::exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        Process::name = words[1];
        Process::status_buffer = words[5]+words[6];
        Process::pP_ID = words[14];
        Process::u_ID = words[19];

        for (int i = 0; i < words.size(); i++)
        {
            if (words[i]== "VmSize:") Process::ram_Usage = words[++i];

        }
        Process::command = Process::Command();
        Process::cpu_Usage = Process::CpuUtilization();
        Process::uptime = Process::UpTime();
    }
    
/***********
    Process::pP_ID = Process::Parent_Pid();
    Process::ram_Usage = Process::Ram();
    Process::status_buffer=Process::Status();
    Process::u_ID = Process::User();
    Process::name = Process::Name();
***********/
}

void            Process::Log(int round,bool flags[9]){

    Process::Update();
    std::string filename = "P_"+to_string(process_ID)+"_Log";
    std::ofstream Log_file (filename.c_str(), std::ofstream::out);
    int i=0;
    Log_file << "SyS Monitor Log File";
    Log_file << "\n PID \t CPU \t MEM \t TIME";
    
    while(i<round){
      Log_file << "\n ";
      Log_file << std::to_string(process_ID).c_str();
      Log_file << "\t";
      Log_file << std::to_string(Process::Read_Cpu()).c_str();
      Log_file << "\t";
      Log_file << Process::Read_Ram().c_str();
      Log_file << "\t";
      Log_file << std::to_string(Process::Read_Uptime()).c_str();
      Log_file << "\t";      
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      i++;
    }
  std::terminate;
}

std::string Process::Read_Name(){
    return name;
}
std::string Process::Read_Parent(){
    return Process::pP_ID;
}
std::string Process::Read_User(){
    return Process::u_ID;
}
std::string Process::Read_Command(){
    return Process::command;
}
std::string Process::Read_Ram(){
    return Process::ram_Usage;
}
std::string Process::Read_Status(){
    return Process::status_buffer;
}
int         Process::Read_Pid(){
    return Process::process_ID;
}
float       Process::Read_Cpu(){
    return Process::cpu_Usage;
}
long int    Process::Read_Uptime(){
    return Process::uptime;
}