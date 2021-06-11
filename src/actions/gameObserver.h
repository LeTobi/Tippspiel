#ifndef TIPPSPIEL_ACTIONS_GAME_OBSERVER_H
#define TIPPSPIEL_ACTIONS_GAME_OBSERVER_H

#include <tobilib/general/exception.hpp>
#include <tobilib/database/database.h>
#include "../misc/time.h"

class GameObserver
{
public:
    tobilib::Logger log;

    void init();
    void tick();

private:
    struct PushInfo {
        Time time;
        tobilib::Database::Cluster game;
    };

    std::vector<PushInfo> recent_push;

    void send_reminders();
    bool is_already_pushed(tobilib::Database::Cluster game);
    bool hits_time_window(Time now, Time start, Time max_dist, Time min_dist) const;
};

#endif