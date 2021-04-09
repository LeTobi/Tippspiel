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

    struct StartOrder {
        bool operator()(const Game*, const Game*);
    };
    struct FinishOrder {
        bool operator()(const Game*, const Game*);
    };

    using Startlist = std::set<Game*, StartOrder>;
    using Finishlist = std::set<Game*, FinishOrder>;
    using StartIterator = Startlist::iterator;
    using FinishIterator = Finishlist::iterator;

    ~GameTimeline();

    void init();
    void tick();
    void update(tobilib::Database::Cluster);
    void remove(tobilib::Database::Cluster);

    std::map<tobilib::Database::Cluster,Game*> games;
    std::set<Game*> pending_games;
    StartIterator upcoming_begin;
    StartIterator upcoming_end;
    FinishIterator running_begin;
    FinishIterator running_end;
    FinishIterator finished_begin;
    FinishIterator finished_end;
    
    Startlist startlist;
    Finishlist finishlist;

private:
    Game* last_ignore = nullptr;

    bool hits_upcoming_horizon(const Game*, const Game*);
    bool hits_finished_horizon(const Game*, const Game*);
    bool has_started(const Game*, const Game*);
    bool is_running(const Game*, const Game*);
    bool has_finished(const Game*, const Game*);
    void set_indicators();

}; // class GameTimeLine


#endif