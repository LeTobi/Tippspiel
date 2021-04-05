#ifndef TIPPSPIEL_SESSIONTASKS_H
#define TIPPSPIEL_SESSIONTASKS_H

#include "update.h"
#include "pong.h"

class Session;

class SessionTasks {
public:

    SessionTasks(Session*);

    void tick_all();
    bool any_pending();

    SessionUpdate update;
    SessionPong pong;

};

#endif