#include "all.h"
#include "pong.h"
#include "update.h"
#include "token.h"
#include "signout.h"
#include "cooldown.h"


SessionTasks::SessionTasks(Session* session):
    update(*new SessionUpdate()),
    pong(*new SessionPong()),
    registration(*new RegisterTask()),
    token_restore(*new RestoreTask()),
    signout(*new SessionSignout()),
    cooldown(*new SessionCooldown())
{
    update.session = session;
    pong.session = session;
    registration.session = session;
    token_restore.session = session;
    signout.session = session;
    cooldown.session = session;
}

SessionTasks::~SessionTasks()
{
    delete &update;
    delete &pong;
    delete &registration;
    delete &token_restore;
    delete &signout;
    delete &cooldown;
}

void SessionTasks::tick()
{
    update.tick();
    pong.tick();
    registration.tick();
    token_restore.tick();
    signout.tick();
    cooldown.tick();
}

bool SessionTasks::pending()
{
    return update.pending()
        || pong.pending()
        || registration.pending()
        || token_restore.pending()
        || signout.pending()
        || cooldown.pending();
}