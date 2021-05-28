#include "cooldown.h"
#include <algorithm>
#include "../session.h"
#include "../main-data.h"

const int SESSION_USAGE_COOLDOWN = 60;
const int LOGIN_COOLDOWN_TIME = 60;
const int FREE_LOGIN_TRIES = 5;

void SessionCooldown::tick()
{
    if (session->status == Session::Status::active)
        usage_last = get_time();
}

bool SessionCooldown::pending() const
{
    return false;
}

int SessionCooldown::session_usage() const
{
    if (get_time() - usage_last < SESSION_USAGE_COOLDOWN)
        return WORK_BUSY;
    else if (session->status == Session::Status::cleanup)
        return WORK_BACKGROUND;
    else
        return WORK_STANDBY;
}

void SessionCooldown::login_success()
{
    login_tries = 0;
    successive_fails = 0;
}

void SessionCooldown::login_fail()
{
    if (get_time()-login_block_time > current_cooldown_time()) {
        login_tries=0;
        //successive_fails=0;
    }
    login_tries++;
    login_block_time = get_time();
    if (login_tries>=FREE_LOGIN_TRIES)
    {
        session->log << FREE_LOGIN_TRIES << " Fehlversuche wurden erreicht." << std::endl;
        login_block_time = get_time() + current_cooldown_time();
        successive_fails++;
        login_tries=0;
    }
}

int SessionCooldown::login_cooldown_get() const
{
    return std::max(0, login_block_time - get_time());
}

int SessionCooldown::current_cooldown_time() const
{
    return LOGIN_COOLDOWN_TIME + 2*LOGIN_COOLDOWN_TIME*successive_fails;
}