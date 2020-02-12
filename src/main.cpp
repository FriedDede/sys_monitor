#include "ncurses_display.h"
#include "system.h"
#include <thread>


void Ncurse_Display_Handler(){
  System system;
  NCursesDisplay::Display(system, 0);
}

int main() {

  std::thread Ncurse_Display_Thread(Ncurse_Display_Handler);
  
  Ncurse_Display_Thread.join();

  return 0;
}
