#ifndef TIPPSPIEL_VIRTUALDB_H
#define TIPPSPIEL_VIRTUALDB_H

#include <tobilib/database/database.h>
#include <tobilib/ssl/h2rfp.h>
#include <vector>
#include <set>
#include "time.h"

class VirtualDB {
public:
    VirtualDB();

    const static Time WAIT_ENDLESS;
    const static Time WAIT_LONG;
    const static Time WAIT_SHORT;
    const static Time WAIT_NOT;

    static int get_table(const std::string&);
    static std::vector<std::string> TableNames;

    tobilib::h2rfp::JSObject get_entry(int, unsigned int);
    int find_entry(int, unsigned int);
    tobilib::h2rfp::JSObject get_history(Time);
    Time get_history_support();
    void clear_cache();

    void update(int, unsigned int, Time = WAIT_ENDLESS);

    struct UpdateEvent {
        std::string type;
        tobilib::h2rfp::JSObject msg;
    };

    tobilib::Queue<UpdateEvent> updates;
    int stat_requests = 0;
    int stat_caches = 0;
    

private:
    struct Entry {
        Entry(int, unsigned int);

        int table;
        unsigned int idx;

        bool operator==(const Entry&) const;
        bool operator!=(const Entry&) const;
        bool operator<(const Entry&) const;
    };

    struct CacheEntry {
        CacheEntry(const Entry&);

        tobilib::h2rfp::JSObject msg;
        Entry dbentry;
    };

    struct HistoryEntry {
        int period_begin;
        int period_end;
        std::set<Entry> entries;
    };

    tobilib::h2rfp::JSObject data(Entry);
    tobilib::h2rfp::JSObject data_user(unsigned int);
    tobilib::h2rfp::JSObject data_group(unsigned int);
    tobilib::h2rfp::JSObject data_player(unsigned int);
    tobilib::h2rfp::JSObject data_team(unsigned int);
    tobilib::h2rfp::JSObject data_event(unsigned int);
    tobilib::h2rfp::JSObject data_game(unsigned int);
    tobilib::h2rfp::JSObject data_eventTipp(unsigned int);
    tobilib::h2rfp::JSObject data_gameTipp(unsigned int);

    std::vector<CacheEntry> cache;
    std::vector<HistoryEntry> history;

    const static int CACHE_SIZE;
    const static Time HISTORY_SIZE;
};

#endif