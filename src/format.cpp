#include <iomanip>
#include <sstream>
#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long minutes, hours;
  minutes = seconds / 60;
  hours = minutes / 60;

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << hours;
  std::string str_hours = ss.str();

  ss.str("");
  ss.clear();

  ss << std::setfill('0') << std::setw(2) << (minutes % 60);
  std::string str_minutes = ss.str();

  ss.str("");
  ss.clear();

  ss << std::setfill('0') << std::setw(2) << (seconds % 60);
  std::string str_seconds = ss.str();

  return str_hours + ":" + str_minutes + ":" + str_seconds;
}