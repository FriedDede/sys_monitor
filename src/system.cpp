#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sys/sysinfo.h>
#include <linux/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <thread>

#include "process.h"
#include "processor.h"
#include "system.h"

#define ESC_KEY 27

std::vector<std::string> System::SysFileread(std::string filename){
    std::vector<std::string> words;
    std::string path= "/proc/" + filename;   
    std::ifstream proc_Sys (path.c_str(), std::ifstream::in);
    std::string str;
    while (proc_Sys >> str){                  
        words.push_back(str);
    }
    proc_Sys.close();        
    return words;
}

std::string System::OperatingSystem() { 
    std::vector<std::string> words = System::SysFileread("sys/kernel/version");
    return words[0]; 
}

std::string System::Hostname() { 
    std::vector<std::string> words = System::SysFileread("sys/kernel/hostname");
    return words[0]; 
}

std::string System::Kernel() {
    std::vector<std::string> words = System::SysFileread("version");
    return words[2]; 
}

Processor& System::Cpu() {
    return cpu_; 
}

int System::TotalProcesses() { 
    struct sysinfo info;
    sysinfo(&info);
    return info.procs; 
}

std::vector<Process>& System::Processes() {

    int process_position=0;
    int process_number_at_runtime = System::TotalProcesses();
    struct dirent *dirp;

    processes_.resize(process_number_at_runtime);
    std::string dir = std::string("/proc");
    std::vector<std::string> path_list;
    DIR *dp;
    dp  = opendir(dir.c_str());

    while ((dirp = readdir(dp)) != NULL){
        path_list.push_back(std::string(dirp->d_name));
    }

    for (auto path : path_list){
        if (atoi(path.c_str())>0)
        {
            int pid = atoi(path.c_str());
            //temp_process.Update();
            if(processes_[process_position].Read_Pid() != pid && process_position<process_number_at_runtime) {
                Process temp_process;
                temp_process.SetPid(pid);            
                processes_[process_position]=temp_process;
            }
            process_position++;
        } 
    }

    processes_.resize(process_position);
    closedir(dp);

    return processes_;
}

float System::MemoryUtilization() { 
    struct sysinfo info;
    sysinfo(&info);
    return ((float)info.totalram-(float)info.freeram)/(float)info.totalram;
}

float System::MemoryBuffer() { 
    struct sysinfo info;
    sysinfo(&info);
    return (float)info.bufferram/(float)info.totalram;
}

float System::MemoryShared() { 
    struct sysinfo info;
    sysinfo(&info);
    return (float)info.sharedram/(float)info.totalram;
}

float System::MemorySwap() { 
    struct sysinfo info;
    sysinfo(&info);
    return ((float)info.totalswap-(float)info.freeswap)/(float)info.totalswap;
}

int System::RunningProcesses() {    
    int running_counter=0;
    for (auto & indx_process : processes_)
    {
        if (indx_process.Read_Status() == "R" )
       {
           running_counter++;
       }
    }
    return running_counter;
}

long int System::UpTime() { 
    struct sysinfo info;
    sysinfo(&info);
    return info.uptime; 
}

System::System(){
    Processor cpu;
    System::cpu_ = cpu;
}