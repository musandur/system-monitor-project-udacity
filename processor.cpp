#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  long total_time = LinuxParser::Jiffies();
  long idle_time = LinuxParser::IdleJiffies();
  
  float cpu_utilization =  static_cast<float> (total_time - idle_time) / idle_time; 

  return cpu_utilization;
}