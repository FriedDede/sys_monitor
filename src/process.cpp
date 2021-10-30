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

// Return a string vector for /proc/filename
std::vector<std::string> Process::procfileread(std::string filename)
{
    std::vector<std::string> words;
    if (Process::Exist())
    {
        std::string str_pid = std::to_string(Process::pid);
        std::string path = "/proc/" + str_pid + "/" + filename;
        std::ifstream proc_pid_status(path.c_str(), std::ifstream::in);
        std::string str;

        while (proc_pid_status >> str)
        {
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
// Return process name
std::string Process::Name()
{
    if (Process::Exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        return words[1];
    }
    else
    {
        return "Not Found";
    }
}
// Return process status
std::string Process::Status()
{
    if (Process::Exist())
    {
        Process::status.clear();
        std::vector<std::string> words = Process::procfileread("status");
        return words[5] + words[6];
    }
    else
    {
        return "Not Found";
    }
}
// Return ppid
std::string Process::ParentPid()
{
    if (Process::Exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        return words[14];
    }
    else
    {
        return "Not Found";
    }
}

void Process::SetPid(int buff_pid)
{
    Process::pid = buff_pid;
}
// Return Cpu utilization, 100% = 1 core full load
float Process::CpuUtilization()
{
    if (Process::Exist())
    {
        float s_time, delta_s_time;
        float u_time, delta_u_time;
        float uptime, delta_uptime;

        std::vector<std::string> stat = procfileread("stat");
        u_time = (float)(atoi(stat[13].c_str())) / sysconf(_SC_CLK_TCK);
        s_time = (float)(atoi(stat[14].c_str())) / sysconf(_SC_CLK_TCK);
        uptime = (float)Process::UpTime();

        delta_s_time = s_time - Process::prev_s_time;
        delta_u_time = u_time - Process::prev_u_time;
        delta_uptime = uptime - Process::prev_uptime;

        Process::prev_uptime = uptime;
        Process::prev_u_time = u_time;
        Process::prev_s_time = s_time;

        return (delta_s_time + delta_u_time) / delta_uptime;
    }
    else
    {
        return 0;
    }
}
// Return the command that generates the process
std::string Process::Command()
{
    if (Process::Exist())
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
std::string Process::Ram()
{
    if (Process::Exist())
    {
        std::vector<std::string> words = Process::procfileread("status");

        for (size_t i = 0; i < words.size(); i++)
        {
            if (words[i] == "VmSize:")
            {
                return words[++i];
            }
        }
    }
    return "Not Found";
}
// Return the user (id) that generated this process
std::string Process::User()
{
    if (Process::Exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        return words[19];
    }
    else
    {
        return "Not Found";
    }
}
// Return the age of this process (in seconds)
long int Process::UpTime()
{

    if (Process::Exist())
    {
        long int time = 0;
        std::vector<std::string> stat = procfileread("stat");
        struct sysinfo info;
        sysinfo(&info);
        time = info.uptime - (atoi(stat[21].c_str()) / sysconf(_SC_CLK_TCK));
        return time;
    }
    else
    {
        return 0;
    }
}
// Return 1 if process exist
bool Process::Exist()
{
    std::string str_pid = std::to_string(Process::pid);
    std::string path = "/proc/" + str_pid + "/status";
    ;
    std::ifstream proc_pid_status(path.c_str(), std::ifstream::in);
    bool file_status = (bool)proc_pid_status;
    return file_status;
}
// Update the data saved in the process instance
void Process::Update()
{
    if (Process::Exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        Process::name = words[1];
        Process::status = words[5] + words[6];
        Process::ppid = words[14];
        Process::uid = words[19];

        for (size_t i = 0; i < words.size(); i++)
        {
            if (words[i] == "VmSize:")
                Process::ram = words[++i];
        }
        Process::command = Process::Command();
        Process::cpu = Process::CpuUtilization();
        Process::uptime = Process::UpTime();
    }
}
// Log to file
void Process::Log(int cycles)
{

    std::string filename = "P_" + std::to_string(pid) + "_Log";
    std::ofstream Log_file(filename.c_str(), std::ofstream::out);
    int i = 0;
    Log_file << "SyS Monitor Log File";
    Log_file << "\n PID \t CPU[%%] \t TMEM[KB] \t TIME[s]";

    while (i < cycles)
    {
        Process::Update();
        Log_file << "\n ";
        Log_file << std::to_string(pid).c_str();
        Log_file << "\t     ";
        Log_file << std::to_string(Process::Read_Cpu() * 100).c_str();
        Log_file << "\t     ";
        Log_file << Process::Read_Ram().c_str();
        Log_file << "\t     ";
        Log_file << std::to_string(Process::Read_Uptime()).c_str();
        Log_file << "\t";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        i++;
    }
    std::terminate();
}

std::string Process::Read_Name()
{
    return name;
}
std::string Process::Read_Parent()
{
    return Process::ppid;
}
std::string Process::Read_User()
{
    return Process::uid;
}
std::string Process::Read_Command()
{
    return Process::command;
}
std::string Process::Read_Ram()
{
    return Process::ram;
}
std::string Process::Read_Status()
{
    return Process::status;
}
int Process::Read_Pid()
{
    return Process::pid;
}
float Process::Read_Cpu()
{
    return Process::cpu;
}
long int Process::Read_Uptime()
{
    return Process::uptime;
}