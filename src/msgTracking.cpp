#include "msgTracking.h"
#include "main-data.h"
#include "filters/all.h"

void global_message_update(tobilib::Database::Cluster cluster, Time urgency)
{
    filters::update(cluster);
    
    if (cluster.type().name == "User") {
        MsgID id (MsgType::user,cluster.index());
        maindata->updateTracker.update(id,urgency);
        maindata->cache.remove(id);
    }
    else if (cluster.type().name == "Group") {
        MsgID id (MsgType::group,cluster.index());
        maindata->updateTracker.update(id,urgency);
        maindata->cache.remove(id);
    }
    else if (cluster.type().name == "Player") {
        MsgID id (MsgType::player,cluster.index());
        maindata->updateTracker.update(id,urgency);
        maindata->cache.remove(id);
    }
    else if (cluster.type().name == "Team") {
        MsgID id (MsgType::team,cluster.index());
        maindata->updateTracker.update(id,urgency);
        maindata->cache.remove(id);
    }
    else if (cluster.type().name == "Event") {
        MsgID id (MsgType::event,cluster.index());
        maindata->updateTracker.update(id,urgency);
        maindata->cache.remove(id);
    }
    else if (cluster.type().name == "Game") {
        MsgID id (MsgType::game,cluster.index());
        maindata->updateTracker.update(id,urgency);
        maindata->cache.remove(id);
    }
    else if (cluster.type().name == "eventTipp") {
        MsgID id (MsgType::eventTipp,cluster.index());
        maindata->updateTracker.update(id,urgency);
        maindata->cache.remove(id);
    }
    else if (cluster.type().name == "gameTipp") {
        MsgID id (MsgType::gameTipp,cluster.index());
        maindata->updateTracker.update(id,urgency);
        maindata->cache.remove(id);
    }
}

void global_message_update(FilterID id)
{
    
}
