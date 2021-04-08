#include "points.h"
#include "../main-data.h"
#include "../msgTracking.h"

using namespace tobilib;

Database::Cluster data_edit::get_rank(Database::Cluster user, Database::Cluster event, bool force)
{
    for (Database::Member rankptr: user["ranks"])
    {
        if (*rankptr["event"] == event)
            return *rankptr;
    }
    if (!force)
        return Database::Cluster();
    
    FlagRequest lock = maindata->storage.begin_critical_operation();
        Database::Cluster rank = maindata->storage.list("Rank").emplace();
        user["ranks"].emplace().set( rank );
        event["ranks"].emplace().set( rank );
        rank["user"].set( user );
        rank["event"].set( event );
    maindata->storage.end_critical_operation(lock);

    global_message_update(event,WAIT_LONG);
    global_message_update(user,WAIT_LONG);

    return rank;
}

void data_edit::tipp_set_points(Database::Cluster tipp, int points)
{
    int old_points_tipp = tipp["reward"].get<int>();
    int old_points_user = tipp["user"]["points"].get<int>();
    int old_points_rank = tipp["rank"]["points"].get<int>();
    int delta_points = points - old_points_tipp;

    FlagRequest lock = maindata->storage.begin_critical_operation();
        tipp["reward"].set(points);
        tipp["user"]["points"].set( old_points_user + delta_points);
        tipp["rank"]["points"].set( old_points_rank + delta_points);
    maindata->storage.end_critical_operation(lock);

    global_message_update(*tipp["user"],WAIT_SHORT);
    global_message_update(*tipp["rank"],WAIT_SHORT);
    global_message_update(tipp, WAIT_SHORT);
}