#ifndef TIPPSPIEL_ACTIONS_GAME_OBSERVER_H
#define TIPPSPIEL_ACTIONS_GAME_OBSERVER_H

#include <tobilib/general/exception.hpp>

class GameObserver
{
public:
    tobilib::Logger log;

    void init();
    void tick();
};

#endif