#include "event.h"
#include "../main-data.h"
#include "../msgTracking.h"
#include "../misc/time.h"
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
            rank["eventtipp"].set( tipp );
        maindata->storage.end_critical_operation(lock);

        global_message_update(rank,WAIT_LONG);
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