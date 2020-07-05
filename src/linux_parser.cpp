#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <experimental/filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::ifstream;
using std::replace;
using std::getline;
using std::istringstream;

namespace fs = std::experimental::filesystem;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      replace(line.begin(), line.end(), ' ', '_');
      replace(line.begin(), line.end(), '=', ' ');
      replace(line.begin(), line.end(), '"', ' ');
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, ignore, kernel;
  string line;
  ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    linestream >> os >> ignore >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  fs::path procDir{kProcDirectory};
  if (fs::exists(procDir) && fs::is_directory(procDir)) {
    for (auto& entry: fs::directory_iterator(procDir)) {
      if (fs::is_directory(entry.status())) {
        auto filename = entry.path().filename().generic_string();
        if (std::all_of(filename.begin(), filename.end(), isdigit)) {
          int pid = stoi(filename);
          pids.push_back(pid);
        }
      }
    }
  }
  return pids;
}

float LinuxParser::MemoryUtilization() {
  float memUtil = 0.0f, memTotal, memFree, value;
  string line, key;
  ifstream ifs{kProcDirectory+kMeminfoFilename};
  if (ifs.is_open()) {
    while (getline(ifs, line)) {
      istringstream is(line);
      is >> key >> value;
      if (key == "MemTotal:") memTotal = value;
      if (key == "MemFree:") memFree = value;
    }
    memUtil = (memTotal - memFree)/memTotal;
  }
  return memUtil;
}

long LinuxParser::UpTime() { 
  ifstream ifs(kProcDirectory + kUptimeFilename); 
  string line, uptime;
  if (ifs.is_open()) {
    getline(ifs, line);
    istringstream iss(line); 
    iss >> uptime;
    return std::stol(uptime);
  }
  return 0;
}

long LinuxParser::Jiffies() {
  string line, cpu_string;
  long value, total_jiffies{0};
  ifstream ifs(kProcDirectory + kStatFilename);
  if (ifs.is_open()) {
    getline(ifs, line);
    istringstream linestream(line);
    linestream >> cpu_string; // remove first string
    while (linestream >> value) {
      total_jiffies += value;
    }
  }
  return total_jiffies;
}

long LinuxParser::ActiveJiffies(int pid) {
  string key, line;
  long int value{0}, active_jiffies{0};
  ifstream ifs(kProcDirectory + to_string(pid) + kStatFilename);
  if (ifs.is_open()) {
    getline(ifs, line);
    istringstream iss(line);
    for (int i = 0; i < 13; ++i) {
      iss >> key;
    }
    for (int i = 0; i < 4; ++i) {
      iss >> value;
      active_jiffies += value;
    }
    return active_jiffies;
  }
  return 0;
}

long LinuxParser::ActiveJiffies() {
  string line, cpu_string;
  long value, active_jiffies{0};
  vector<int> cpu_data{};
  ifstream ifs(kProcDirectory + kStatFilename);
  if (ifs.is_open()) {
    getline(ifs, line);
    istringstream iss(line);
    iss >> cpu_string;  // remove first string
    while (iss >> value) {
      cpu_data.push_back(value);
    }
  }
  active_jiffies =
      cpu_data.at(CPUStates::kUser_) + cpu_data.at(CPUStates::kNice_) +
      cpu_data.at(CPUStates::kSystem_) + cpu_data.at(CPUStates::kIRQ_) +
      cpu_data.at(CPUStates::kSoftIRQ_) + cpu_data.at(CPUStates::kSteal_);
  return active_jiffies;
}

long LinuxParser::IdleJiffies() {
  string line, cpu_string;
  long value, idle_jiffies{0};
  vector<int> cpu_data{};
  ifstream ifs(kProcDirectory + kStatFilename);
  if (ifs.is_open()) {
    getline(ifs, line);
    istringstream iss(line);
    iss >> cpu_string;  // remove first string
    while (iss >> value) {
      cpu_data.push_back(value);
    }
  }
  idle_jiffies =
      cpu_data.at(CPUStates::kIdle_) + cpu_data.at(CPUStates::kIOwait_);
  return idle_jiffies;
}

vector<string> LinuxParser::CpuUtilization() {
  ifstream ifs(kProcDirectory + kStatFilename);
  string line;
  if (ifs.is_open()) {
    getline(ifs, line);
    istringstream lineStream(line);
    vector<string> cpu_util_info{std::istream_iterator<string>{lineStream}, 
    std::istream_iterator<string>{}};
    cpu_util_info.erase(cpu_util_info.begin());
    return cpu_util_info; 
  }
  return {};
}

int LinuxParser::TotalProcesses() {
  string line, key;
  int value;
  ifstream ifs{kProcDirectory+kStatFilename};
  if (ifs.is_open()) {
    while (getline(ifs, line)) {
      istringstream iss(line);
      iss >> key >> value;
      if (key == "processes") return value;
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line, key;
  int value;
  ifstream ifs{kProcDirectory+kStatFilename};
  if (ifs.is_open()) {
    while (getline(ifs, line)) {
      istringstream iss(line);
      iss >> key >> value;
      if (key == "procs_running") return value;
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  ifstream ifs(kProcDirectory + "/" + to_string(pid) + kCmdlineFilename); 
  string line, cmdline;
  if (ifs.is_open()) {
    getline(ifs, cmdline);
    return cmdline;
  }
  return string{};
}

string LinuxParser::Ram(int pid) {
  string line, key;
  long ram_kB;
  ifstream ifs{kProcDirectory+ to_string(pid)+kStatusFilename};
  // std::cout << kProcDirectory+ to_string(pid)+kStatusFilename << std::endl;
  if (ifs.is_open()) {
    while (getline(ifs, line)) {
      istringstream iss(line);
      iss >> key >> ram_kB;
      if (key == "VmSize:") return to_string(ram_kB/1000);
    }
  }
  return "0";
}

string LinuxParser::Uid(int pid) {
  string line, key, value;
  ifstream ifs{kProcDirectory+ "/" + to_string(pid)+kStatusFilename};
  if (ifs.is_open()) {
    while (getline(ifs, line)) {
      istringstream iss(line);
      iss >> key >> value;
      if (key == "Uid:") return value;
    }
  }
  return string{};
}

string LinuxParser::User(int pid) {
  ifstream ifs{kPasswordPath};
  string line, user_name, x, user_id;
  if (ifs.is_open()) {
    while (getline(ifs, line)) {
      replace(line.begin(), line.end(), ':', ' ');
      istringstream iss(line);
      iss >> user_name >> x >> user_id;
      if (user_id == LinuxParser::Uid(pid)) {
        return user_name;
      }
    }
  }
  return string{};
}

long LinuxParser::UpTime(int pid) {
  ifstream ifs(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  if (ifs.is_open()) {
    getline(ifs, line);
    istringstream iss(line);
    vector<string> data{std::istream_iterator<string>{iss}, std::istream_iterator<string>{}};
    return LinuxParser::UpTime() - std::stol(data[21])/sysconf(_SC_CLK_TCK);
  }
  return 0;
}