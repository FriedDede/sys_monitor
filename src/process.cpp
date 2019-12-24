#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

vector<string> Process::procfileread(std::string filename){
       std::vector<std::string> words;
    if (Process::exist())
    {
        std::string str_pid = std::to_string(Process::process_ID);
        std::string path= "/proc/" + str_pid + "/" + filename;   
        char fln[path.size() + 1];
        path.copy(fln, path.size() +1);
        fln[path.size()]= '\0';
        std::ifstream proc_pid_status (fln, std::ifstream::in);
        std::string str;

        while (proc_pid_status >> str){                  
            words.push_back(str);
        }
        proc_pid_status.close();        
        return words;
    }
    else
    {
        std::string NotFounfFlag = "Not Found";
        words.push_back(NotFounfFlag);
        return words;
    }
}

string Process::Name() {
    if (Process::exist())
    {
        std::vector<std::string> words = Process::procfileread("status");
        string procname;
        for (int i = 0; i < words.size(); i++){
            if (words[i]== "Name:"){
                procname=words[++i];
            }
        }
        return procname;
    }
    else{return "Not Found";} 
}

string Process::status(){
    if (Process::exist())
    {
        Process::status_buffer.clear();
        std::vector<std::string> words = Process::procfileread("status");
        for (int i = 0; i < words.size(); i++)
        {
            if (words[i]== "State:")
            {
                Process::status_buffer=words[++i] + words[i+2];
            }
        }
        return Process::status_buffer;;
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
            if (words[i]== "PPid:"){string PPid = words[++i];
            return PPid;}
        }
    }
    else
    {
        return "Not Found";
    } 
}    

int Process::Pid_Insec(int buff_pid){
    Process::process_ID=buff_pid;
    return 0;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    return 0; 
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
        std::string mem;
        for (int i = 0; i < words.size(); i++)
        {
            if (words[i]== "VmSize:")
            {
                mem=words[++i];
            }
        }
        return mem;
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
        std::string Uid;
        for (int i = 0; i < words.size(); i++)
        {
            if (words[i]== "Uid:")
            {
                Uid=words[++i];
            }
        }
        return Uid;
    }
    else
    {
        return "Not Found";
    } 
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
    return 0; 
}

bool Process::exist(){
    std::string str_pid = std::to_string(Process::process_ID);
    std::string path= "/proc/" + str_pid + "/status";   
    char fln[path.size() + 1];
    path.copy(fln, path.size() +1);
    fln[path.size()]= '\0';
    std::ifstream proc_pid_status (fln, std::ifstream::in);
    bool filestatus= (bool)proc_pid_status;
    return filestatus;
}