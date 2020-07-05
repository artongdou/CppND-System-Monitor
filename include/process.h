#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid, long totalJiffies);
  int Pid() const;                               // TODO: See src/process.cpp
  std::string User() const;                      // TODO: See src/process.cpp
  std::string Command() const;                   // TODO: See src/process.cpp
  float CalcCpuUtilization(long);
  float CpuUtilization() const;                  // TODO: See src/process.cpp
  std::string Ram() const;                       // TODO: See src/process.cpp
  long int UpTime() const;                       // TODO: See src/process.cpp
  bool operator<(Process const& rhs) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
  int pid_ = 0;
  float cpuUtil = 0;
  enum processInfo {
    utime_, 
    stime_, 
    cutime_, 
    cstime_, 
    starttime_
  };
};

#endif