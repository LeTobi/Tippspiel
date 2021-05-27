#include "time.h"

const int MINUTE = 60;
const int HOUR = 60*MINUTE;
const int DAY = 24*HOUR;

int get_time()
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::string interval_to_string(Time t)
{
    t = abs(t);
    if (t > 7*DAY) {
        // day only
        return std::to_string(t/DAY)+"d";
    }
    else if (t > 25*HOUR) {
        // day, hour
        int d = t/DAY;
        int h = (t%DAY)/HOUR;
        return std::to_string(d)+"d "+std::to_string(h)+"h";
    }
    else if (t > 7*HOUR) {
        // hour only
        return std::to_string(t/HOUR)+"h";
    }
    else if (t > 100*MINUTE) {
        // hour, minute
        int h = t/MINUTE;
        int min = (t%HOUR)/MINUTE;
        return std::to_string(h)+"h "+std::to_string(min)+"min";
    }
    else if (t > 7*MINUTE) {
        // minute only
        return std::to_string(t/MINUTE)+"min";
    }
    else if (t > 120) {
        // minute, second
        int min = t/MINUTE;
        int s = t%MINUTE;
        return std::to_string(min)+"min "+std::to_string(s)+"s";
    }
    else
    {
        // second only
        return std::to_string(t)+"s";
    }
}