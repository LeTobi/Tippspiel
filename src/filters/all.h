#ifndef TIPPSPIEL_FILTERS_ALL_H
#define TIPPSPIEL_FILTERS_ALL_H

#include <tobilib/database/database.h>

class GameTimeline;

class DatabaseFilters
{
public:

    DatabaseFilters();
    DatabaseFilters(const DatabaseFilters&) = delete;
    ~DatabaseFilters();

    void init();
    void tick();
    void update(tobilib::Database::Cluster);
    void remove(tobilib::Database::Cluster);

    GameTimeline& timeline;

};

#endif