#include "points.h"
#include "../main-data.h"
#include "../msgTracking.h"
#include "../misc/enums.h"

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

void data_edit::tipp_set_result(
    Database::Cluster tipp,
    int kategorie,
    int goals,
    bool penaltyBonus)
{
    int points = goals;
    switch (kategorie)
    {
    case TIPPKAT_WRONG:
        break;
    case TIPPKAT_TEAM:
        points+=1;
        break;
    case TIPPKAT_DIFF:
        points+=2;
        break;
    case TIPPKAT_EXACT:
        points+=4;
        break;
    }
    if (penaltyBonus)
        points+=1;

    int old_points_tipp = tipp["reward"].get<int>();
    int old_points_user = tipp["user"]["points"].get<int>();
    int old_points_rank = tipp["rank"]["points"].get<int>();
    int delta_points = points - old_points_tipp;

    FlagRequest lock = maindata->storage.begin_critical_operation();
        tipp["tippkat"].set( kategorie );
        tipp["goals"].set( goals );
        tipp["penaltyBonus"].set( penaltyBonus );
        tipp["reward"].set(points);
        tipp["user"]["points"].set( old_points_user + delta_points);
        tipp["rank"]["points"].set( old_points_rank + delta_points);
    maindata->storage.end_critical_operation(lock);

    global_message_update(*tipp["user"],WAIT_SHORT);
    global_message_update(*tipp["rank"],WAIT_SHORT);
    global_message_update(tipp, WAIT_SHORT);
}