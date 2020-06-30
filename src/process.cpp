#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sys/sysinfo.h>

#include "process.h"
#include "system.h"

using std::string;
using std::to_string;
using std::vector;

vector<string> Process::procfileread(std::string filename){
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

string Process::Name() {
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

string Process::Status(){
    if (Process::exist())
    {
        Process::status_buffer.clear();
        std::vector<std::string> words = Process::procfileread("status");
        for (int i = 0; i < words.size(); i++)
        {
            if (words[i]== "State:")
            {
                return words[++i]+" "+words[i+2];
            }
        }
    }
    else
    {
        return "Not Found";
    }  
}

int Process::Pid() {
    return process_ID;     
}

std::string Process::Parent_Pid(){
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

void Process::Pid_Insec(int buff_pid){
    Process::process_ID=buff_pid;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
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

    return (delta_s_time+delta_u_time)/uptime;
}

// Return the command that generates the process   
string Process::Command() {
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
string Process::Ram() {
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
string Process::User() {
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
long int Process::UpTime() { 
    long int time;
    vector<string>stat=procfileread("stat");
    struct sysinfo info;
    sysinfo(&info);
    time=info.uptime-(atoi(stat[21].c_str())/sysconf(_SC_CLK_TCK));
    return time; 
}

bool Process::exist(){
    std::string str_pid = std::to_string(Process::process_ID);
    std::string path= "/proc/" + str_pid + "/status";   ;
    std::ifstream proc_pid_status (path.c_str(), std::ifstream::in);
    bool filestatus= (bool)proc_pid_status;
    return filestatus;
}

void Process::Update(){

    Process::name = Process::Name();
    Process::command = Process::Command();
    Process::cpu_Usage = Process::CpuUtilization();
    Process::pP_ID = Process::Parent_Pid();
    Process::ram_Usage = Process::Ram();
    Process::status_buffer=Process::Status();
    Process::uptime = Process::UpTime();
    Process::u_ID = Process::User();

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
int Process::Read_Pid(){
    return Process::process_ID;
}
float Process::Read_Cpu(){
    return Process::cpu_Usage;
}
long int Process::Read_Uptime(){
    return Process::uptime;
}