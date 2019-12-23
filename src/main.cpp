#include "ncurses_display.h"
#include "system.h"
#include <thread>


int main() {
  
  System system;
  //std::thread Display (NCursesDisplay::Display, system);

  NCursesDisplay::Display(system, system.Processes().size());
}