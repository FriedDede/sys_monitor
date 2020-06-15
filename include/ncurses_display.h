#pragma once
#define NCURSES_DISPLAY_H

#include <curses.h>
#include <thread>
#include "process.h"
#include "system.h"

namespace NCursesDisplay {
    void Display(System& system, int n = 10);
    void DisplayMenu(WINDOW* window);
    void DisplaySystem(System& system, WINDOW* window);
    void DisplayHelp(WINDOW* window);
    void DisplayLogger(WINDOW* window);
    void Process_Logger(int,std::string,int); 
    void DisplayProcesses(std::vector<Process>& processes, WINDOW* window, int n);
    std::string ProgressBar(float percent);
}; 
