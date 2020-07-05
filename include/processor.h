#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <array>
#include <vector>
#include "linux_parser.h"

using LinuxParser::CPUTime;

class Processor {
 public:
  Processor();
  float Utilization();
  long getTotalJiffies();

 private:
  void CalculateDeltaTime(std::array<long, CPUTime::CntCPUTime>&, std::array<long, CPUTime::CntCPUTime>&);
  virtual void GetLinuxParsesForJiffies(std::array<long, CPUTime::CntCPUTime>&);
  float CalculateUtilization();
  void UpdatePrevJiffies();

  std::array<long, CPUTime::CntCPUTime> current_cpu_jiffies_;
  std::array<long, CPUTime::CntCPUTime> prev_cpu_jiffies_;
  std::array<float, CPUTime::CntCPUTime> delta_jiffies_;
};

#endif