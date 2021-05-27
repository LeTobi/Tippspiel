#ifndef TIPPSPIEL_TIME_H
#define TIPPSPIEL_TIME_H

#include <chrono>
#include <string>

using Time = int;

Time get_time();

std::string interval_to_string(Time t);

#endif