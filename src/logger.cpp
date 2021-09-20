#include "process.h"
#include "logger.h"

void Logger::Proc(int pid, int cycles){
    Process tolog;
    tolog.PidInsec(pid);
    tolog.Log(cycles);
}