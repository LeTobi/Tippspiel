#include "msgTracking.h"
#include "main-data.h"
#include "filters/all.h"
#include "updateTracker.h"
#include "msgCache.h"

void global_message_update(MsgType type, unsigned int arg0, Time urgency)
{
    MsgID id (type,arg0);
    maindata->updateTracker.update(id, urgency);
    maindata->cache.remove(id);
}

void global_message_update(tobilib::Database::Cluster cluster, Time urgency)
{
    maindata->log << "update urgency: " << urgency << std::endl;
    maindata->filters.update(cluster);
    
    if (cluster.type().name == "User") {
        global_message_update(MsgType::user,cluster.index(),urgency);
    }
    else if (cluster.type().name == "Group") {
        global_message_update(MsgType::group,cluster.index(),urgency);
    }
    else if (cluster.type().name == "Player") {
        global_message_update(MsgType::player,cluster.index(),urgency);
    }
    else if (cluster.type().name == "Team") {
        global_message_update(MsgType::team,cluster.index(),urgency);
    }
    else if (cluster.type().name == "Event") {
        global_message_update(MsgType::event,cluster.index(),urgency);
    }
    else if (cluster.type().name == "Game") {
        global_message_update(MsgType::game,cluster.index(),urgency);
    }
    else if (cluster.type().name == "eventTipp") {
        global_message_update(MsgType::eventTipp,cluster.index(),urgency);
    }
    else if (cluster.type().name == "gameTipp") {
        global_message_update(MsgType::gameTipp,cluster.index(),urgency);
    }
}

void global_message_update(FilterID id)
{
    switch (id)
    {
    case FilterID::games_finished:
        global_message_update(MsgType::hotGames,0,WAIT_SHORT);
        break;
    case FilterID::games_pending:
        // no message yet
        break;
    case FilterID::games_running:
        global_message_update(MsgType::hotGames,0,WAIT_SHORT);
        break;
    case FilterID::games_upcoming:
        global_message_update(MsgType::hotGames,0,WAIT_SHORT);
        break;
    }
}