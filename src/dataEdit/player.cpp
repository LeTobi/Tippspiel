#include "player.h"
#include "../main-data.h"
#include "../filters/all.h"
#include "../msgTracking.h"

using namespace tobilib;

Database::Cluster data_edit::create_player(
    Database::Cluster user,
    std::string name,
    Database::Cluster team)
{
    FlagRequest lock = maindata->storage.begin_critical_operation();
        Database::Cluster player = maindata->storage.list("Player").emplace();
        player["creator"].set( user );
        player["createdAt"].set( get_time() );
        player["name"].set( name );
        player["team"].set( team );
        player["active"].set( true );
        team["players"].emplace().set( player );
    maindata->storage.end_critical_operation(lock);

    maindata->log << "Spieler hinzugefügt: " << name << std::endl;

    global_message_update(player, WAIT_LONG);
    global_message_update(team, WAIT_LONG);
    global_message_update(FilterID::player_suggestions);

    return player;
}