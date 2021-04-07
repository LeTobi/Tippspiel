#include "all.h"
#include "pong.h"
#include "update.h"
#include "token.h"


SessionTasks::SessionTasks(Session* session):
    update(*new SessionUpdate()),
    pong(*new SessionPong()),
    registration(*new RegisterTask()),
    token_restore(*new RestoreTask())
{
    update.session = session;
    pong.session = session;
    registration.session = session;
    token_restore.session = session;
}

SessionTasks::~SessionTasks()
{
    delete &update;
    delete &pong;
    delete &registration;
    delete &token_restore;
}

void SessionTasks::tick()
{
    update.tick();
    pong.tick();
    registration.tick();
    token_restore.tick();
}

bool SessionTasks::pending()
{
    return update.pending()
        || pong.pending()
        || registration.pending()
        || token_restore.pending();
}