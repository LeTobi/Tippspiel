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
        bool operator()(const Game*, const Game*) const;
    };
    struct FinishOrder {
        bool operator()(const Game*, const Game*) const;
    };

    using Startlist = std::set<Game*, StartOrder>;
    using Finishlist = std::set<Game*, FinishOrder>;

    ~GameTimeline();

    void init();
    void tick();
    void update(tobilib::Database::Cluster);
    void remove(tobilib::Database::Cluster);

    std::map<tobilib::Database::Cluster,Game*> games;
    Startlist future;     // in far future
    Startlist upcoming;  // near future
    Finishlist running;  // running
    Finishlist pending;  // not running anymore but unknown result
    Finishlist finished; // recently finished

private:
    const StartOrder startorder = StartOrder();
    const FinishOrder finishorder = FinishOrder();

    bool has_begun(Game*,Game*);
    bool has_finished(Game*,Game*);

}; // class GameTimeLine


#endif