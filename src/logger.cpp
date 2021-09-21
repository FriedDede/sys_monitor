#include "process.h"
#include "logger.h"

void Logger::Proc(int pid, int cycles){
    Process process;
    process.SetPid(pid);
    process.Log(cycles);
}