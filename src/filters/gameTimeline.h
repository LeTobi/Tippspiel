#ifndef TIPPSPIEL_FILTER_TIMELINE_GAME_H
#define TIPPSPIEL_FILTER_TIMELINE_GAME_H

#include <tobilib/database/database.h>
#include "../misc/time.h"
#include <map>
#include <set>
#include <vector>

class GameTimeline
{
public:
    struct Game {
        Game(tobilib::Database::Cluster);
        Game(Time);
        void update();

        tobilib::Database::Cluster cluster;
        Time starttime;
        Time endtime;
    };

    struct TimeOrder {
        bool operator()(const Game*, const Game*);
    };

    using Timeline = std::set<Game*, TimeOrder>;
    using Iterator = Timeline::iterator;

    ~GameTimeline();

    void init();
    void tick();
    void update(tobilib::Database::Cluster);
    void remove(tobilib::Database::Cluster);

    std::map<tobilib::Database::Cluster,Game*> games;
    Iterator future_horizon;
    Iterator next_game;
    Iterator past_horizon;
    Timeline timeline;

private:

    void set_indicators();

}; // class GameTimeLine


#endif