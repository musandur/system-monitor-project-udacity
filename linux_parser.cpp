#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem()
{
    string line;
    string key;
    string value;
    std::ifstream filestream(kOSPath);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::replace(line.begin(), line.end(), ' ', '_');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '"', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> value)
            {
                if (key == "PRETTY_NAME")
                {
                    std::replace(value.begin(), value.end(), '_', ' ');
                    return value;
                }
            }
        }
    }
    return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel()
{
    string os, kernel, version;
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open())
    {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
    }
    return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids()
{
    vector<int> pids;
    DIR *directory = opendir(kProcDirectory.c_str());
    struct dirent *file;
    while ((file = readdir(directory)) != nullptr)
    {
        // Is this a directory?
        if (file->d_type == DT_DIR)
        {
            // Is every character of the name a digit?
            string filename(file->d_name);
            if (std::all_of(filename.begin(), filename.end(), isdigit))
            {
                int pid = stoi(filename);
                pids.push_back(pid);
            }
        }
    }
    closedir(directory);
    return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
    float valueMemTotal, valueMemFree, result;
    std::ifstream filename(kProcDirectory + kMeminfoFilename);
    std::string keymTotal, keymFree;
    std::string line1, line2;

    if (filename.is_open())
    {
        std::getline(filename, line1);
        std::replace(line1.begin(), line1.end(), ':', ' ');
        std::istringstream linestream1(line1);
        linestream1 >> keymTotal >> valueMemTotal;

        std::getline(filename, line2);
        std::replace(line2.begin(), line2.end(), ':', ' ');
        std::istringstream linestream2(line2);
        linestream1 >> keymFree >> valueMemFree;

        result = static_cast<float>((valueMemTotal - valueMemFree) / valueMemTotal);

        return result;
    }

    return 0.0;
}
// TODO: Read and return the system uptime
long LinuxParser::UpTime()
{

    std::ifstream filename(kProcDirectory + kUptimeFilename);
    std::string line;
    long system_uptime = 0;

    if (filename.is_open())
    {
        std::getline(filename, line);
        std::istringstream linestream(line);
        std::string temp;
        linestream >> temp;

        system_uptime = std::stoi(temp);

        return system_uptime;
    }

    return system_uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{

    return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid)
{
    std::ifstream filename(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (filename.is_open())
    {
        std::string line;
        std::getline(filename, line);
        std::istringstream linestream(line);
        long values[2];
        string jiffies;
        for (int i = 1; i <= 15; i++)
        {
            linestream >> jiffies;
            if (i == 14)
            {
                values[0] = std::stoi(jiffies);
            }
            else if (i == 15)
            {
                values[1] = std::stoi(jiffies);
            }
        }

        return values[0] + values[1];
    }
    return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
    std::vector<std::string> jiffies_vector = LinuxParser::CpuUtilization();

    long active_jiffies = std::stoi(jiffies_vector[CPUStates::kUser_]) +
                          std::stoi(jiffies_vector[CPUStates::kSystem_]) +
                          std::stoi(jiffies_vector[CPUStates::kNice_]) +
                          std::stoi(jiffies_vector[CPUStates::kIRQ_]) +
                          std::stoi(jiffies_vector[CPUStates::kSoftIRQ_]) +
                          std::stoi(jiffies_vector[CPUStates::kSteal_]);

    return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
    std::vector<std::string> jiffies_vector = LinuxParser::CpuUtilization();

    long idle_jiffies = std::stoi(jiffies_vector[CPUStates::kIdle_]) + std::stoi(jiffies_vector[CPUStates::kIOwait_]);

    return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
    std::ifstream filename(kProcDirectory + kStatFilename);
    std::vector<std::string> aggreg_jiffies{};
    std::string cpu, value;
    if (filename.is_open())
    {
        std::string line;
        std::getline(filename, line);
        std::istringstream linestream(line);
        linestream >> cpu;
        if (cpu == "cpu")
        {
            while (linestream >> value)
            {
                aggreg_jiffies.push_back(value);
            }

            return aggreg_jiffies;
        }
    }

    return aggreg_jiffies;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
    std::ifstream filename(kProcDirectory + kStatFilename);
    std::string line;
    std::string process;
    int value;
    if (filename.is_open())
    {
        while (std::getline(filename, line))
        {
            std::istringstream streamlines(line);
            while (streamlines >> process >> value)
            {
                if (process == "processes")
                {
                    return value;
                }
            }
        }
    }
    return value;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
    std::string line;
    std::string process;
    int value;
    std::ifstream filename(kProcDirectory + kStatFilename);
    if (filename.is_open())
    {
        while (std::getline(filename, line))
        {
            std::istringstream streamlines(line);
            while (streamlines >> process >> value)
            {
                if (process == "procs_running")
                {
                    return value;
                }
            }
        }
    }

    return value;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid)
{
    std::ifstream filename(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
    std::string line;
    if (filename.is_open())
    {
        std::getline(filename, line);
        return line;
    }
    return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid)
{
    std::ifstream filename(kProcDirectory + std::to_string(pid) + kStatusFilename);
    std::string line, key, ram;
    if (filename.is_open())
    {
        while (std::getline(filename, line))
        {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> ram)
            {
                if (key == "VmSize")
                {
                    return ram;
                }
            }
        }
    }
    return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid)
{
    std::ifstream filename(kProcDirectory + std::to_string(pid) + kStatusFilename);
    std::string line, key, uid;
    if (filename.is_open())
    {
        while (std::getline(filename, line))
        {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> uid)
            {
                if (key == "Uid")
                {
                    return uid;
                }
            }
        }
    }
    return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid)
{
    std::string uid = LinuxParser::Uid(pid);
    std::ifstream filename(kPasswordPath);
    std::string line, username, x, uid_user;
    if (filename.is_open())
    {
        while (std::getline(filename, line))
        {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            while (linestream >> username >> x >> uid_user)
            {
                if (uid_user == uid)
                {
                    return username;
                }
            }
        }
    }

    return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid)
{
    std::ifstream filename(kProcDirectory + std::to_string(pid) + kStatFilename);
    std::string line;
    long process_uptime = 0;

    if (filename.is_open())
    {
        std::getline(filename, line);
        std::istringstream linestream(line);
        std::string temp;
        for (int i = 1; i <= 22; i++)
        {
            linestream >> temp;
            if (i == 22)
            {
                process_uptime = std::stoi(temp);
            }
        }

        return process_uptime;
    }

    return process_uptime;
}
