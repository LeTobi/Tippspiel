#include "all.h"
#include "gameObserver.h"

ServerActions::ServerActions():
    gameObserver(*new GameObserver())
{ }

ServerActions::~ServerActions()
{
    delete &gameObserver;
}

void ServerActions::init()
{
    gameObserver.init();
}

void ServerActions::tick()
{
    gameObserver.tick();
}