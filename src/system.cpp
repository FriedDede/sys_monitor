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

using std::set;
using std::size_t;
using std::string;


std::vector<string> System::Sysfileread(std::string filename){
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
    std::string file="sys/kernel/version";
    std::vector<std::string> words = System::Sysfileread(file);
    return words[0]; 
}

std::string System::Hostname() { 
    std::string file="sys/kernel/hostname";
    std::vector<std::string> words = System::Sysfileread(file);
    return words[0]; 
}

std::string System::Kernel() {
    std::string file= "version";
    std::vector<std::string> words = System::Sysfileread(file);
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
    std::string dir = string("/proc");
    std::vector<string> files;
    DIR *dp;
    dp  = opendir(dir.c_str());

    while ((dirp = readdir(dp)) != NULL){
        files.push_back(string(dirp->d_name));
    }

    for (int i = 2; i < files.size(); i++){
        std::string path = files[i];
        
        if (atoi(path.c_str())>0)
        {
            int pid = atoi(path.c_str());
            Process temp_process;
            temp_process.Pid_Insec(pid);
            if(processes_[process_position].Pid() != pid) processes_[process_position]=temp_process;
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
    float ramusage;
    float freeram=info.freeram;
    float totalram=info.totalram;
    ramusage = ((totalram-freeram)/totalram);
    return ramusage;
}


int System::RunningProcesses() {    
    int running_counter=0;
    std::string running_status= "R(running)";
    for (int i = 0; i < processes_.size(); i++)
    {
       if (processes_[i].status() == running_status )
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