#include <string>

#include "linux_parser.h"
#include "processor.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { return LinuxParser::CpuUtilization(); }