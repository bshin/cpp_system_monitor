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

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// setter function for Pid
void Process::Pid(int pid) { pid_ = pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return (float)LinuxParser::ActiveJiffies(pid_) / (float)LinuxParser::UpTime(pid_); }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
// COMPARISON REG: CPU UTILIZATION
bool Process::operator<(Process const& a) const {
    return a.CpuUtilization() < this->CpuUtilization();  // order wrt. cpu utilization
    // return stoi(a.Ram()) < stoi(this->Ram());  // order wrt. ram utilization
}