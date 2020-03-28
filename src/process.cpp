#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// DONE: Return this process's ID
int Process::Pid() const { return Process::pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const {
  return LinuxParser::CpuUtilization(this->Pid());
}

// DONE: Return the command that generated this process
string Process::Command() const { return LinuxParser::Command(this->Pid()); }

// DONE: Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(this->Pid()); }

// DONE: Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(this->Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(this->Pid()); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return a.CpuUtilization() < CpuUtilization();
}