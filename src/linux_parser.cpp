#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

// #include <iostream>
#include <string>
#include <vector>

#include "format.h"

// #define printVariableNameAndValue(x) std::cout<<"The name of variable **"<<(#x)<<"** and the value of variable is => "<<x<<"\n"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
// using namespace std;


template <typename T>
T findValueByKey(string const &keyFilter, string const &filename) {
  string line, key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  // stream.close();
  return value;
};


template <typename T>
T getValueOfFile(std::string const &filename) {
  std::string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  // stream.close();
  return value;
};

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string memTotal = "MemTotal:";
  string memFree = "MemFree:";
  float total = findValueByKey<float>(memTotal, kMeminfoFilename);
  float free = findValueByKey<float>(memFree, kMeminfoFilename);
  return (total - free) / total;
}

/*
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  string kb;  // for kB string at end of line
  float memTotal;
  float memAvbl;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> kb) {
        // get total memory
        if (key == "MemTotal") {
          memTotal = std::stof(value);
        }
        // get available memory
        if (key == "MemAvailable") {
          memAvbl = std::stof(value);
          return (memTotal - memAvbl) /
                 memTotal;  // return utilized memory as ratio
        }
      }
    }
  }
  return 0.0;
} */

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string uptime;
  string totalUpTime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> uptime >> totalUpTime) {
        return std::stol(uptime);  // return system uptime in seconds
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string line;
  long jiffy;
  long totalJiffies {0};
  string rest;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    for (int i=0; i<11; i++) {  // go thru all entries of first line
      if (i > 0) {  
        linestream >> jiffy;
        totalJiffies = totalJiffies + jiffy;
      }
      else {
        linestream >> rest;
      }
    }
  }
  return totalJiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  long jiffy;
  long activeJiffies {0};
  string rest;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    for (int i=0; i<17; i++) {  // go thru line entries 
      if ( i >= 13 && i <= 16 ) {  // look for entries with active jiffies (14, 15, 16, 17)
        linestream >> jiffy;
        activeJiffies = activeJiffies + jiffy;
      }
      else {
        linestream >> rest;
      }
    }
  }
  return activeJiffies / sysconf(_SC_CLK_TCK);  // return active jiffies in seconds  
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line;
  long jiffy;
  long activeJiffies {0};
  string rest;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    for (int i=0; i<11; i++) {  // go thru line entries 
      if (i > 0 && i != 4 && i !=5 ) {  // look for entries with active jiffies
        linestream >> jiffy;
        activeJiffies = activeJiffies + jiffy;
      }
      else {
        linestream >> rest;
      }
    }
  }
  return activeJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line;
  long jiffy;
  long idleJiffies {0};
  string rest;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    for (int i=0; i<11; i++) {  // go thru line entries
      if (i == 4 || i == 5 ) {  // look for 4th and 5th entry which give idle jiffies
        linestream >> jiffy;
        idleJiffies = idleJiffies + jiffy;
      }
      else {
        linestream >> rest;
      }
    }
  }
  return idleJiffies;
 }

// TODO: Read and return CPU utilization
// vector<string> LinuxParser::CpuUtilization() {
// only return total cpu utilization, therefore replace vector<string> by float number
float LinuxParser::CpuUtilization() { 
  return (float)LinuxParser::ActiveJiffies()/(float)LinuxParser::Jiffies();  // compute ratio between active and total jiffies
}

/*
// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {   // look for key processes
          return std::stoi(value);  // return number of total processes
        }
      }
    }
  }
  return 0;
}
*/

int LinuxParser::TotalProcesses() {
  string key = "processes";
  return findValueByKey<int>(key, kStatFilename);
}

/*
// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {  // look for running processes key
          return std::stoi(value);     // return number of running processes
        }
      }
    }
  }
  return 0;
}*/

int LinuxParser::RunningProcesses() {
  string key = "procs_running";
  return findValueByKey<int>(key, kStatFilename);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
/*
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::replace(line.begin(), line.end(), ' ',
                 '_');  // TODO: not sure if the complete command is printed...
    std::getline(filestream, line);
  }
  return line;
}*/

string LinuxParser::Command(int pid) {
  return string(getValueOfFile<string>(to_string(pid)+kCmdlineFilename));
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string key = "VmData:";  // used VmData instead of VmSize since VmSize is the sum of complete virtual memory
  int ramValue = findValueByKey<int>(key, to_string(pid) + kStatusFilename);
  return to_string(ramValue / 1024);  
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string key = "Uid:";
  return findValueByKey<string>(key, to_string(pid) + kStatusFilename);
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uidString = LinuxParser::Uid(pid);
  string line;
  string username {};
  string userid;
  string rest;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');  // replace :
      std::replace(line.begin(), line.end(), 'x', ' ');  // replace x
      std::istringstream linestream(line);
      while (linestream >> username >> userid >> rest) {
        if (userid == uidString) {  // look for correct uid
          return username;
        }        
      }
      linestream.clear();
    }
  }
  filestream.close();
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  string rest;
  long uptime;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    for (int i=0; i<22; i++) {  // go thru all entries
      if (i == 21) {  // look for 22nd entry which gives starttime in cycles
        linestream >> uptime;
      }
      else {
        linestream >> rest;
      }
    }

  }
  filestream.close(); 
  return LinuxParser::UpTime() - uptime / sysconf(_SC_CLK_TCK);  // convert starttime into seconds and subtract from system uptime in seconds
}
