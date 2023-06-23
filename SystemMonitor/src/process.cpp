#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

#include <iostream>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  pid_ = pid;
}

// Returning this process's ID
int Process::Pid() const { return pid_; }

// Returning this process's CPU utilization
float Process::CpuUtilization() const {
  float act_time = float(LinuxParser::ActiveJiffies(Pid()))/100.f;
  float up_time = float(LinuxParser::UpTime(Pid()));
  float util = act_time/up_time;
  return float(util);
}

// Returning the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// Returning this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(Pid()); }

// Returning the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// Returning the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// Overloading the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return stol(Ram()) < stol(a.Ram()); }
