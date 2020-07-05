#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <array>
#include <vector>
#include "linux_parser.h"

using std::array;
using std::vector;
using LinuxParser::CPUTime;

class Processor {
 public:
  Processor();
  float Utilization();
  long getTotalJiffies();

 private:
  void CalculateDeltaTime(array<long, CPUTime::CntCPUTime>&, array<long, CPUTime::CntCPUTime>&);
  virtual void GetLinuxParsesForJiffies(array<long, CPUTime::CntCPUTime>&);
  float CalculateUtilization();
  void UpdatePrevJiffies();

  array<long, CPUTime::CntCPUTime> current_cpu_jiffies_;
  array<long, CPUTime::CntCPUTime> prev_cpu_jiffies_;
  array<float, CPUTime::CntCPUTime> delta_jiffies_;
};

#endif