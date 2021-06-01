#ifndef TIPPSPIEL_SESSIONTASKS_COOLDOWN_H
#define TIPPSPIEL_SESSIONTASKS_COOLDOWN_H

#include "../misc/time.h"

class Session;

class SessionCooldown
{
public:
    Session* session;

    void tick();
    bool pending() const;

    int session_usage() const;
    void login_fail();
    void login_success();
    int login_cooldown_get() const;

private:
    int login_tries = 0;
    int successive_fails = 0;
    Time login_block_time = 0;
    Time usage_last = 0;

    int current_cooldown_time() const;
};

#endif