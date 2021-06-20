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

void data_edit::tipp_set_result(Database::Cluster tipp, bool team, bool diff, bool exact, bool draw, int scorer)
{
    int points = scorer;
    if (team)
        points += 1;
    if (diff)
        points += 1;
    if (exact)
        points += 2;
    if (draw)
        points += 2;

    int old_points_tipp = tipp["reward"].get<int>();
    int old_points_rank = tipp["rank"]["points"].get<int>();
    int delta_points = points - old_points_tipp;

    FlagRequest lock = maindata->storage.begin_critical_operation();
        tipp["reward_team"].set( team );
        tipp["reward_diff"].set( diff );
        tipp["reward_exact"].set( exact );
        tipp["reward_draw"].set( draw );
        tipp["reward_scorer"].set( scorer );
        tipp["reward"].set(points);
        tipp["rank"]["points"].set( old_points_rank + delta_points );
    maindata->storage.end_critical_operation(lock);

    global_message_update(*tipp["user"],WAIT_SHORT);
    global_message_update(*tipp["rank"],WAIT_SHORT);
    global_message_update(tipp, WAIT_SHORT);
}

void data_edit::evtipp_set_result(Database::Cluster evtipp, bool team, bool scorer)
{
    int points = 0;
    if (team)
        points += 3;
    if (scorer)
        points += 3;
    
    int old_points_tipp = evtipp["reward"].get<int>();
    int old_points_rank = evtipp["rank"]["points"].get<int>();
    int delta_points = points - old_points_tipp;

    FlagRequest lock = maindata->storage.begin_critical_operation();
        evtipp["reward_winner"].set( team );
        evtipp["reward_topscorer"].set( scorer );
        evtipp["reward"].set( points );
        evtipp["rank"]["points"].set( old_points_rank + delta_points );
    maindata->storage.end_critical_operation(lock);

    global_message_update(*evtipp["user"] , WAIT_SHORT);
    global_message_update(*evtipp["rank"], WAIT_SHORT);
    global_message_update(evtipp, WAIT_SHORT);
}