#include "all.h"

SessionTasks::SessionTasks(Session* session)
{
    update.session = session;
    pong.session = session;
}

void SessionTasks::tick_all()
{
    update.tick();
    pong.tick();
}

bool SessionTasks::any_pending()
{
    return update.pending()
        || pong.pending();
}