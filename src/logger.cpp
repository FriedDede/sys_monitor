#include "process.h"
#include "logger.h"

void Logger::Proc(int pid, int cycles){
    Process tolog;
    tolog.Pid_Insec(pid);
    tolog.Log(cycles);
}