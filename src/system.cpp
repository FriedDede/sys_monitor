#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>


#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;


vector<string> System::Sysfileread(std::string filename){
       std::vector<std::string> words;
        std::string path= "/proc/" + filename;   
        char fln[path.size() + 1];
        path.copy(fln, path.size() +1);
        fln[path.size()]= '\0';
        std::ifstream proc_Sys (fln, std::ifstream::in);
        std::string str;
        while (proc_Sys >> str){                  
            words.push_back(str);
        }
        proc_Sys.close();        
        return words;
}
// TODO: Return the system's CPU
Processor& System::Cpu() {
    return cpu_; 
}

int System::TotalProcesses() { 
    struct sysinfo info;
    sysinfo(&info);
    return info.procs; 
}

// Return a vector structure containing processes list
vector<Process>& System::Processes() {
    int process_position=0;  
    int process_number_at_runtime = System::TotalProcesses();
    processes_.resize(process_number_at_runtime);
    string dir = string("/proc");
    vector<string> files;
    DIR *dp;
    struct dirent *dirp;
    dp  = opendir(dir.c_str());
    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    for (int i = 2; i < files.size(); i++)
    {
        std::string path = files[i];
        char fln[path.size() + 1];
        path.copy(fln, path.size() +1);
        fln[path.size()]= '\0';
        
        if (atoi(path.c_str())>0)
        {   
            int pid= atoi(path.c_str());
            Process temp_process;
            temp_process.Pid_Insec(pid);
            processes_[process_position]=temp_process;   
            process_position++;
        }        
    }
    processes_.resize(process_position);
    closedir(dp);
    return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {

    std::vector<std::string> words;
    
    std::string path= "/proc/version";  
    char fln[path.size() + 1];
    path.copy(fln, path.size() +1);
    fln[path.size()]= '\0';
    std::ifstream proc_pid_status (fln, std::ifstream::in);
    std::string str;

    while (proc_pid_status >> str){                  
        words.push_back(str);
    }
    proc_pid_status.close(); 

    return words[2]; 
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


float System::Cpumean1m() { 
    struct sysinfo info;
    sysinfo(&info);
    //output format must be 0.usage;
    //info.load range is 0 to 1000;
    float avgload=info.loads[0]/100000.0;
    return avgload;
}

// TODO: Return the operating system name
std::string System::OperatingSystem() { 
    std::vector<std::string> words;
    
    std::string path= "/proc/sys/kernel/version";  
    char fln[path.size() + 1];
    path.copy(fln, path.size() +1);
    fln[path.size()]= '\0';
    std::ifstream proc_pid_status (fln, std::ifstream::in);
    std::string str;

    while (proc_pid_status >> str){                  
        words.push_back(str);
    }
    proc_pid_status.close(); 

    return words[0]; 
}

std::string System::Hostname() { 
    std::vector<std::string> words;
    
    std::string path= "/proc/sys/kernel/hostname";  
    char fln[path.size() + 1];
    path.copy(fln, path.size() +1);
    fln[path.size()]= '\0';
    std::ifstream proc_pid_status (fln, std::ifstream::in);
    std::string str;

    while (proc_pid_status >> str){                  
        words.push_back(str);
    }
    proc_pid_status.close(); 

    return words[0]; 
}
// TODO: Return the number of processes actively running on the system
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