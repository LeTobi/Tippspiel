#ifndef TIPPSPIEL_VIRTUALDB_H
#define TIPPSPIEL_VIRTUALDB_H

#include <tobilib/database/database.h>
#include <tobilib/ssl/h2rfp.h>
#include <vector>
#include <set>
#include "time.h"

class VirtualDB {
public:
    struct Entry {
        Entry(int, unsigned int);
        Entry(const tobilib::Database::Cluster&);

        int table;
        unsigned int idx;

        operator tobilib::Database::Cluster() const;
        bool operator==(const Entry&) const;
        bool operator!=(const Entry&) const;
        bool operator<(const Entry&) const;
    };

    struct Update {
        tobilib::h2rfp::JSObject data;
        Time time;
    };

    VirtualDB();

    const static Time WAIT_ENDLESS;
    const static Time WAIT_LONG;
    const static Time WAIT_SHORT;
    const static Time WAIT_NOT;

    static int get_table(const std::string&);
    static std::vector<std::string> TableNames;

    tobilib::h2rfp::JSObject get_data(Entry);
    int find_entry(Entry);
    tobilib::h2rfp::JSObject get_history(Time);
    Time get_history_support();
    void clear_cache();

    void update(Entry, Time);
    void tick();

    tobilib::Queue<Update> updates;
    int stat_requests = 0;
    int stat_caches = 0;
    

private:
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

    tobilib::h2rfp::JSObject set_to_update(const std::set<Entry>&);
    HistoryEntry& history_now();
    void history_advance();
    tobilib::h2rfp::JSObject data(Entry);
    tobilib::h2rfp::JSObject data_user(tobilib::Database::Cluster);
    tobilib::h2rfp::JSObject data_group(tobilib::Database::Cluster);
    tobilib::h2rfp::JSObject data_player(tobilib::Database::Cluster);
    tobilib::h2rfp::JSObject data_team(tobilib::Database::Cluster);
    tobilib::h2rfp::JSObject data_event(tobilib::Database::Cluster);
    tobilib::h2rfp::JSObject data_game(tobilib::Database::Cluster);
    tobilib::h2rfp::JSObject data_eventTipp(tobilib::Database::Cluster);
    tobilib::h2rfp::JSObject data_gameTipp(tobilib::Database::Cluster);

    std::vector<CacheEntry> cache;
    std::vector<HistoryEntry> history;

    const static int CACHE_SIZE;
    const static Time HISTORY_SIZE;
};

#endif