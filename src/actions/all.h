#ifndef TIPPSPIEL_ACTIONS_ALL_H
#define TIPPSPIEL_ACTIONS_ALL_H

class GameObserver;

class ServerActions
{
public:
    ServerActions();
    ServerActions(const ServerActions&) = delete;
    ~ServerActions();

    void init();
    void tick();
    
    GameObserver& gameObserver;

};

#endif