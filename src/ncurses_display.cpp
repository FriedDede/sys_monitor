#include <curses.h>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include <iostream>
#include <fstream>

#include "format.h"
#include "ncurses_display.h"
#include "system.h"

#define ESC_KEY 27


using std::string;
using std::to_string;

void NCursesDisplay::Process_Logger(int process_id,std::string path,int cyclenumber){
    
    Process process;
    process.Pid_Insec(process_id);   
    char fln[path.size() + 1];
    path.copy(fln, path.size() +1);
    fln[path.size()]= '\0';
    std::ofstream Log_file (fln, std::ofstream::out);
    Log_file << "SyS Monitor Log File";
    Log_file << "\n PID";
    int i=0;

    while(i<cyclenumber){
      Log_file << "\n ";
      Log_file << std::to_string(process.Pid()).c_str();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      i++;
    }
    std::cout <<"\n Process "<<process_id<<" logged\n";
  std::terminate;       
}

// 50 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string NCursesDisplay::ProgressBar(float percent) {
  std::string result{"0%"};
  int size{50};
  float bars{percent * size};

  for (int i{0}; i < size; ++i) {
    result += i <= bars ? '|' : ' ';
  }

  string display{to_string(percent * 100).substr(0, 4)};
  if (percent < 0.1 || percent == 1.0)
    display = " " + to_string(percent * 100).substr(0, 3);
  return result + " " + display + "/100%";
}
void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
  int row{0};
  mvwprintw(window, ++row, 2, "                         SyS Monitor V0.1");
  mvwprintw(window, ++row, 2, ("OS: " + system.OperatingSystem()).c_str());
  mvwprintw(window, ++row, 2, ("Kernel: " + system.Kernel()).c_str());
  mvwprintw(window, ++row, 2, ("Hostname: " + system.Hostname()).c_str());
  //CPU usage
  mvwprintw(window, ++row, 2, "CPU: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(system.Cpu().Utilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  //CPU 1m average printer
  mvwprintw(window, ++row, 2, "CPU avg: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(system.Cpumean1m()).c_str());
  wattroff(window, COLOR_PAIR(1));
  //Memory usage printer
  mvwprintw(window, ++row, 2, "Memory: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(system.MemoryUtilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2,
            ("Total Processes: " + to_string(system.TotalProcesses())).c_str());
  mvwprintw(
      window, ++row, 2,
      ("Running Processes: " + to_string(system.RunningProcesses())).c_str());
  mvwprintw(window, ++row, 2,
            ("Up Time: " + Format::ElapsedTime(system.UpTime())).c_str());
  wrefresh(window);
}
void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes,WINDOW* window, int n) {
    
    int row{0};
    int const pid_column{2};
    int const Ppid_column{11};
    int const user_column{20};
    int const cpu_column{27};
    int const ram_column{35};
    int const time_column{44};
    int const status_column{54};
    int const command_column{67};
    
    wattron(window, COLOR_PAIR(2));
    werase(window);
    mvwprintw(window, row, pid_column, "PID");
    mvwprintw(window, row, Ppid_column, "PPID");
    mvwprintw(window, row, user_column, "UID");
    mvwprintw(window, row, cpu_column, "CPU[%%]");
    mvwprintw(window, row, ram_column, "RAM[KB]");
    mvwprintw(window, row, time_column, "TIME+");
    mvwprintw(window, row, status_column, "STATUS");
    mvwprintw(window, row, command_column, "COMMAND");
    wattroff(window, COLOR_PAIR(2));
  
  for (int i = n-1; i > 0; i--) {
    
      if (processes[i].exist())
      {
      mvwprintw(window, ++row, pid_column, to_string(processes[i].Pid()).c_str());
      
      mvwprintw(window, row, Ppid_column, processes[i].Parent_Pid().c_str());
      
      mvwprintw(window, row, user_column, processes[i].User().c_str());
      float cpu = processes[i].CpuUtilization() * 100;
      mvwprintw(window, row, cpu_column, to_string(cpu).substr(0, 4).c_str());
      
      mvwprintw(window, row, ram_column, processes[i].Ram().c_str());
      mvwprintw(window, row, time_column,
                Format::ElapsedTime(processes[i].UpTime()).c_str());
      
      mvwprintw(window, row, status_column, processes[i].status().c_str());
      
      mvwprintw(window, row, command_column,
                processes[i].Command().substr(0, window->_maxx - 46).c_str());
      }  
  }
}
void NCursesDisplay::DisplayMenu(WINDOW* window){
  mvwprintw(window, 1, 2, ("L:Logger    H: Help   ESC:Exit"));
}
void NCursesDisplay::DisplayHelp(WINDOW* window){
  wtimeout(window,-1);
  werase(window);
  mvwprintw(window, 1, 2, ("SyS Monitor v0.1"));
  mvwprintw(window, 2, 2, ("Developer: Andrea Motta"));
  mvwprintw(window, 3, 2, ("GitHub: https://github.com/Andrea759/system_monitor"));
  mvwprintw(window, 4, 2, ("Forked from: https://github.com/udacity/CppND-System-Monitor"));
  mvwprintw(window, 5, 2, ("Version: 2020-01-05"));
  mvwprintw(window, 7, 2, ("Press Any key to continue."));
  wgetch(window);
  fflush(stdin);
  werase(window);
}
void NCursesDisplay::DisplayLogger(WINDOW* window){
  wtimeout(window,-1);
    werase(window);
    echo();
    mvwprintw(window, 1, 2, ("SyS Monitor Logger v0.1"));
    mvwprintw(window, 2, 2, ("Developer: Andrea Motta"));
    mvwprintw(window, 3, 2, ("GitHub: https://github.com/Andrea759/system_monitor"));
    mvwprintw(window, 4, 2, ("Forked from: https://github.com/udacity/CppND-System-Monitor"));
    mvwprintw(window, 5, 2, ("Version: 2020-01-05"));
    mvwprintw(window, 7, 2, ("Press Enter"));
    wgetch(window);
  endwin();

  int pid;
  //mvwprintw(window, 8, 2, ("Insert Pid"));
  std::cout << "\nInsert Pid: ";
  std::cin >> pid;
  std::cout << "\nInsert n of Logs:  ";
  int Lognumber;
  std::cin >> Lognumber;
  string filename = "P_"+to_string(pid)+"_Log";
  std::thread Logger(Process_Logger,pid,filename, Lognumber);
  Logger.detach();
  //wgetch(window);
  getch();
  fflush(stdin);
  //werase(window);
  //noecho();

}

void NCursesDisplay::Display(System& system,int n) {
  initscr();      // start ncurses
  noecho();       // do not print input values
  cbreak();       // terminate ncurses on ctrl + c
  start_color();  // enable color
  int key_pressed=0;
  int x_max{getmaxx(stdscr)};
  int y_max{getmaxy(stdscr)};
  int system_window_size=12;
  int menu_window_size=3;
  WINDOW* system_window = newwin(system_window_size, x_max - 1, 0, 0);
  WINDOW* process_window = newwin(y_max-system_window_size-menu_window_size, x_max - 1, system_window->_maxy , 0);
  WINDOW* menu_window = newwin(menu_window_size, x_max -1, process_window->_maxy + system_window->_maxy +2, 0);
  keypad(process_window,TRUE);
  keypad(menu_window,TRUE);

  while (1) {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    //Getting terminal x and y 
    int x_max_temp{getmaxx(stdscr)};
    int y_max_temp{getmaxy(stdscr)};
    //Windows sizes and properties
    system_window = newwin(system_window_size, x_max_temp - 1, 0, 0);
    process_window = newwin(y_max_temp-system_window_size-menu_window_size, x_max_temp - 1, system_window->_maxy+1 , 0);
    menu_window = newwin(menu_window_size, x_max_temp -1, process_window->_maxy + system_window->_maxy +2, 0);
    //Boxes borders
    box(system_window, 0, 0);
    box(menu_window, 0, 0);
    //Display functions

    //std::thread Display_System_Thread(DisplaySystem, system, system_window);
    
    DisplaySystem(system, system_window);
    DisplayProcesses(system.Processes(), process_window, system.Processes().size());
    DisplayMenu(menu_window);
    //Active windows resizing
    wrefresh(system_window);
    wrefresh(process_window);
    wrefresh(menu_window);
    //Non blocking wgetch
    wtimeout(stdscr,0);
    key_pressed=wgetch(stdscr);
    //wgetch processing

    if(key_pressed=='h'){DisplayHelp(process_window);fflush(stdin);}
    if(key_pressed=='l'){DisplayLogger(process_window);fflush(stdin);}
    if(key_pressed==ESC_KEY){fflush(stdin);endwin();return;}

    
    else{fflush(stdin);}
    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
  }
  endwin();
}