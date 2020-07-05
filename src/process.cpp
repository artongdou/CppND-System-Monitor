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

Process::Process(int pid, long totalJiffies) : pid_(pid), cpuUtil(CalcCpuUtilization(totalJiffies)) {}

int Process::Pid() const { return pid_; }

float Process::CalcCpuUtilization(const long totalJiffies) {
    long activeJiffies = LinuxParser::ActiveJiffies(pid_);
    return (activeJiffies * 1.0 / totalJiffies);
}

float Process::CpuUtilization() const { return cpuUtil; }

string Process::Command() const { return LinuxParser::Command(pid_); }

string Process::Ram() const { return LinuxParser::Ram(pid_); }

string Process::User() const { return LinuxParser::User(pid_); }

long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& rhs) const {
    return this->cpuUtil > rhs.cpuUtil;
}