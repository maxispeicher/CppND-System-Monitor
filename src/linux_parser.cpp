#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string line, os, version, kernel;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

vector<Process> LinuxParser::Processes() {
  vector<Process> processes;

  vector<int> pids = LinuxParser::Pids();
  for (int pid : pids) {
    Process process = Process(pid);
    processes.push_back(process);
  }

  return processes;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  int value, mem_total{-1}, mem_free{-1};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          mem_total = value;
        } else if (key == "MemFree") {
          mem_free = value;
        }

        if (mem_total != -1 && mem_free != -1) {
          return (float)(mem_total - mem_free) / mem_total;
        }
      }
    }
  }
  return 0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime{0};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// DONE: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
  string cpu, line;
  int user, nice, system, idle, iowait, irq, softirq, steal;
  float utilization{0};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal;

    int total_idle = idle + iowait;
    int total_non_idle = user + nice + system + irq + softirq + steal;

    utilization = (float)total_non_idle / (total_idle + total_non_idle);
  }
  return utilization;
}

float LinuxParser::CpuUtilization(int pid) {
  float utilization{0};
  string line, value;
  int utime, stime, cutime, cstime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++) {
      linestream >> value;
    }

    linestream >> utime >> stime >> cutime >> cstime;

    int total_time = utime + stime + cutime + cstime;
    utilization = (float)total_time / LinuxParser::UpTime(pid);
  }

  return utilization;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, line;
  int processes{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes") {
        linestream >> processes;
        return processes;
      }
    }
  }
  return processes;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, line;
  int processes{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {
        linestream >> processes;
        return processes;
      }
    }
  }
  return processes;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid[[maybe_unused]]) {
  string command{""};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  std::getline(filestream, command);
  return command;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string key, line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
    linestream >> key;
    if (key == "VmSize:") {
      int ram;
      linestream >> ram;
      ram = ram / 1024;
      return std::to_string(ram);
    }
  }
  return 0;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key, line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
    linestream >> key;
    if (key == "Uid:") {
      string uid;
      linestream >> uid;
      return uid;
    }
  }
  return "";
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid_to_search_for = LinuxParser::Uid(pid);

  string line, current_uid;
  std::ifstream filestream(kPasswordPath);
  while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
    unsigned first_delimiter, second_delimiter, third_delimitor;
    first_delimiter = line.find(":");
    second_delimiter = line.find(":", first_delimiter + 1);
    third_delimitor = line.find(":", second_delimiter + 1);

    current_uid = line.substr(second_delimiter + 1,
                              third_delimitor - second_delimiter - 1);
    if (uid_to_search_for == current_uid) {
      return line.substr(0, first_delimiter);
    }
  }
  return "";
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, value;
  long uptime{0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 23; i++) {
      linestream >> value;
    }
    long start_time = std::stol(value);
    start_time = start_time / sysconf(_SC_CLK_TCK);
    uptime = LinuxParser::UpTime() - start_time;
    return uptime;
  }
  return uptime;
}