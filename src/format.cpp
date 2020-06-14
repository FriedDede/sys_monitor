#include <string>
#include <time.h>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS

string Format::ElapsedTime(long int seconds) { 

    time_t rawtime = seconds;
    struct tm *ptm = gmtime(&rawtime);


    std::string H = std::to_string(ptm->tm_hour).c_str();
    std::string M = std::to_string(ptm->tm_min).c_str();
    std::string S = std::to_string(ptm->tm_sec).c_str();

    std::string time = H + ":" + M + ":" + S;
    
    return time; 
}