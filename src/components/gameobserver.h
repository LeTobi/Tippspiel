#ifndef TIPPSPIEL_GAMEOBSERVER_H
#define TIPPSPIEL_GAMEOBSERVER_H

#include <tobilib/database/database.h>
#include <vector>
#include <set>
#include <map>
#include "time.h"

class GameObserver {
public:
    using Game = tobilib::Database::Cluster;

    GameObserver();
    void setup();

    std::vector<Game> get_upcoming();
    std::vector<Game> get_finished();

    // aufgerufen von VirtualDB
    void update(Game);
    
    void tick();

private:
    struct KnownGame {
        KnownGame();
        KnownGame(const Game&);

        Game game;
        Time starttime = -1;
        Time endtime = -1;
    };

    struct TimeOrder{
        bool operator()(const KnownGame&, const KnownGame&) const;
    };
    struct DataOrder {
        bool operator()(const Game&, const Game&) const;
    };

    const static Time DURATION;

    KnownGame next_start;
    KnownGame next_end;
    void get_next_game();

    using DataSorter = std::map<Game,KnownGame,DataOrder>;
    using TimeSorter = std::set<KnownGame,TimeOrder>;
    TimeSorter timeline;
    DataSorter knowngames;
};

#endif