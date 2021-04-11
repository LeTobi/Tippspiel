#ifndef TIPPSPIEL_MSG_CACHE_H
#define TIPPSPIEL_MSG_CACHE_H

#include <vector>
#include <tobilib/protocols/h2rfp.h>
#include "misc/msgID.h"

class Session;

class MsgCache
{
public:
    const static int CACHE_SIZE;

    tobilib::h2rfp::JSObject get_data(Session&, const MsgID&);
    int find_entry(const MsgID&) const;
    int size() const;
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
};

#endif