#ifndef TIPPSPIEL_UPDATE_TRACKER_H
#define TIPPSPIEL_UPDATE_TRACKER_H

#include "misc/time.h"
#include "misc/msgID.h"
#include <tobilib/protocols/h2rfp.h>
#include <set>

const Time WAIT_ENDLESS = 3600;
const Time WAIT_LONG = 60;
const Time WAIT_SHORT = 10;
const Time WAIT_NOT = 0;

class UpdateTracker
{
public:
    UpdateTracker();
    ~UpdateTracker();

    void tick();
    void update(MsgID, Time);
    Time horizon() const;
    tobilib::h2rfp::JSObject get_history(Time) const;

    tobilib::Logger log;

private:

    struct HistoryEntry {
        Time period_begin = get_time();
        Time period_end = get_time()+24*60*60;
        std::set<MsgID> changes;
    };

    tobilib::h2rfp::JSObject make_msg(const std::set<MsgID>&) const;

    HistoryEntry* current;
    std::vector<HistoryEntry*> history;

    const static Time HISTORY_SIZE;

};

#endif