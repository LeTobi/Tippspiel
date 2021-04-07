#ifndef TIPPSPIEL_ACTIONS_GAME_OBSERVER_H
#define TIPPSPIEL_ACTIONS_GAME_OBSERVER_H

#include "../misc/time.h"

class GameObserver
{
public:
    void init();
    void tick();

private:
    Time horizon = 0;
};

#endif