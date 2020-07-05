#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long HH, MM, SS;
    HH = seconds/3600;
    seconds -= HH*3600;
    MM = seconds/60;
    seconds -= MM*60;
    SS = seconds;
    return PadZero(HH)+":"+PadZero(MM)+":"+PadZero(SS);
}

string Format::PadZero(long value) {
    string XX = std::to_string(value);
    while (XX.size() < 2) {
        XX = "0" + XX;
    }
    return XX;
}