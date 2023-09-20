#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// constructor
Process::Process(int pid_) : pid{pid_} {}
// TODO: Return this process's ID
int Process::Pid()
{
    return pid;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const
{
    long system_uptime = LinuxParser::UpTime();
    long process_uptime = LinuxParser::UpTime(pid);

    // user time in clock ticks
    long utime = LinuxParser::ActiveJiffies(pid);
    // system time or kernel mode time in clock ticks
    long stime = LinuxParser::ActiveJiffies(pid) - utime;

    // calculate the totale time the process has been running in clock ticks
    long total_time = utime + stime;
    // convert the total time in seconds
    total_time = total_time / sysconf(_SC_CLK_TCK);

    // total seconds since the process started
    long seconds = system_uptime - (process_uptime / sysconf(_SC_CLK_TCK));

    float cpuUsage = static_cast<float>(total_time / seconds);
    cpuUsage += 100;

    return cpuUsage;
}

// TODO: Return the command that generated this process
string Process::Command()
{
    return LinuxParser::Command(pid);
}

// TODO: Return this process's memory utilization
string Process::Ram()
{
    return LinuxParser::Ram(pid);
}

// TODO: Return the user (name) that generated this process
string Process::User()
{
    return LinuxParser::User(pid);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime()
{
    return LinuxParser::UpTime(pid);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const &a) const
{
    return CpuUtilization() < a.CpuUtilization();
}