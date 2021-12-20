#include <string>
#include <iomanip>
#include <sstream>

#include "format.h"

using std::string;
using namespace std;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {    
    int hrs = seconds / 3600;  // get full hours
    seconds = seconds - hrs * 3600;  // get remaining seconds
    int mins = seconds / 60; // get minutes
    seconds = seconds - mins * 60;  // get remaining seconds
    int secs = (int)seconds;

    stringstream formattedTime;  
    // build string stream with time data, fill with zeros if string has less than 2 characters
    formattedTime << setfill('0') << setw(2) << hrs << ':' << setw(2) << mins << ':' << setw(2) << secs;    
    return formattedTime.str();
}