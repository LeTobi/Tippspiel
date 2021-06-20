#include "event.h"
#include "../main-data.h"
#include "../msgTracking.h"
#include "../misc/time.h"
#include "../misc/enums.h"
#include "points.h"

using namespace tobilib;

Database::Cluster data_edit::get_event_tipp(Database::Cluster user, Database::Cluster event, bool force)
{
    Database::Cluster rank = data_edit::get_rank(user,event,force);
    Database::Cluster tipp = *rank["eventtipp"];

    if (tipp.is_null())
    {
        if (!force)
            return tipp;
        
        FlagRequest lock = maindata->storage.begin_critical_operation();
            tipp = maindata->storage.list("EventTipp").emplace();
            tipp["user"].set( user );
            tipp["event"].set( event );
            tipp["rank"].set( rank );
            rank["eventtipp"].set( tipp );
        maindata->storage.end_critical_operation(lock);

        global_message_update(rank,WAIT_LONG);
        global_message_update(tipp,WAIT_ENDLESS);
        global_message_update(event,WAIT_LONG);
    }

    return tipp;
}

void data_edit::set_event_tipp(Database::Cluster tipp, Database::Cluster winnerteam, Database::Cluster topscorer)
{
    tipp["winner"].set( winnerteam );
    tipp["topscorer"].set( topscorer );
    tipp["createdAt"].set( get_time() );

    global_message_update(tipp,WAIT_ENDLESS);
}

void data_edit::report_event(Database::Cluster event, Database::Cluster user, Database::Cluster winnerteam, Database::Cluster topscorer)
{
    event["reporter"].set( user );
    event["winner"].set( winnerteam );
    event["topscorer"].set( topscorer );
    event["eventStatus"].set( ESTATUS_ENDED );

    event_evaluate(event);
    global_message_update(event, WAIT_SHORT);
}

void data_edit::event_evaluate(Database::Cluster event)
{
    Database::Cluster winner = *event["winner"];
    Database::Cluster topscorer = *event["topscorer"];
    
    FlagRequest lock = maindata->storage.begin_critical_operation();
        for (Database::Member rank: event["ranks"])
        {
            Database::Cluster tipp = *rank["eventtipp"];
            if (tipp.is_null())
                continue;
            bool team_right = winner==*tipp["winner"];
            bool scorer_right = topscorer==*tipp["topscorer"];
            evtipp_set_result(tipp,team_right,scorer_right);
        }
    maindata->storage.end_critical_operation(lock);

    global_message_update(event,WAIT_NOT);
}