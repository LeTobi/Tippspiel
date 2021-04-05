#ifndef TIPPSPIEL_MSG_CACHE_H
#define TIPPSPIEL_MSG_CACHE_H

#include <vector>
#include <tobilib/protocols/h2rfp.h>
#include "misc/msgID.h"
#include "session.h"

class MsgCache
{
public:
    tobilib::h2rfp::JSObject get_data(Session&, const MsgID&);
    int find_entry(const MsgID&);
    void clear();
    void remove(const MsgID&);

    int stat_requests = 0;
    int stat_hits = 0;

private:
    struct Entry {
        Entry(const MsgID&);
        tobilib::h2rfp::JSObject data;
        MsgID id;
    };

    std::vector<Entry> entries;

    const static int CACHE_SIZE;
};

#endif