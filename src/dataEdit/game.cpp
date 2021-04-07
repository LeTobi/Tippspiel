#include "game.h"
#include "../main-data.h"
#include "../msgTracking.h"
#include "../misc/enums.h"

using namespace tobilib;

Database::Cluster data_edit::get_game_tipp(
    Database::Cluster user,
    Database::Cluster game,
    bool force)
{
    for (Database::Member tipp: game["tipps"])
    {
        if (*tipp["user"] == user)
            return *tipp;
    }
    if (!force)
        return Database::Cluster();
    
    FlagRequest lock = maindata->storage.begin_critical_operation();
        Database::Cluster tipp = maindata->storage.list("GameTipp").emplace();
        user["gameTipps"].emplace().set( tipp );
        game["tipps"].emplace().set( tipp );
        tipp["user"].set( user );
        tipp["game"].set( game );
    maindata->storage.end_critical_operation(lock);

    global_message_update(tipp, WAIT_LONG);
    global_message_update(user, WAIT_LONG);
    global_message_update(game, WAIT_SHORT);

    return tipp;
}

void data_edit::set_game_tipp(
    Database::Cluster tipp,
    int bet1,
    int bet2,
    Database::Cluster winner,
    Database::Cluster topscorer)
{
    // kein kritischer zugriff
    tipp["createdAt"].set( get_time() );
    tipp["bet"][0].set( bet1 );
    tipp["bet"][1].set( bet2 );
    tipp["winner"].set( winner );
    tipp["topscorer"].set( topscorer );

    // Die Änderung ist nicht öffentlich sichtbar
    global_message_update(tipp, WAIT_ENDLESS);
}

Database::Cluster data_edit::get_location(
    Database::Cluster user,
    const std::string& name)
{
    if (name.empty())
        return Database::Cluster();

    for (Database::Cluster loc: maindata->storage.list("Location"))
        if (loc["name"].get<std::string>()==name)
            return loc;
    
    Database::Cluster loc = maindata->storage.list("Location").emplace();
    loc["creator"].set( user );
    loc["createdAt"].set( get_time() );
    loc["name"].set( name );

    global_message_update(loc, WAIT_LONG);

    return loc;
}

Database::Cluster data_edit::create_game()
{
    return maindata->storage.list("Game").emplace();
}

void data_edit::announce_game(
    tobilib::Database::Cluster user,
    tobilib::Database::Cluster game,
    const Time start,
    tobilib::Database::Cluster location,
    tobilib::Database::Cluster event,
    const std::string& name,
    const std::string& stream,
    tobilib::Database::Cluster team1,
    tobilib::Database::Cluster team2,
    tobilib::Database::Cluster previousStage1,
    tobilib::Database::Cluster previousStage2
    )
{
    Database::Cluster old_prev_1 = *game["previousStage"][0];
    Database::Cluster old_prev_2 = *game["previousStage"][1];
    Database::Cluster old_event = *game["event"];

    FlagRequest lock = maindata->storage.begin_critical_operation();
        game["creator"].set( user );
        game["createdAt"].set( get_time() );
        game["start"].set( (int)start );
        game["location"].set( location );
        game["name"].set( name );
        game["stream"].set( stream );
        if (*game["event"] != event)
        {
            game["event"]["games"].erase( game );
            game["event"].set( event );
            event["games"].emplace().set( game );
        }
        game["phase"].set( GAMEPHASE_NORMAL );
        game["teams"][0].set( team1 );
        game["teams"][1].set( team2 );
        game["previousStage"][0]["nextStage"].set( Database::Cluster() );
        game["previousStage"][1]["nextStage"].set( Database::Cluster() );
        game["previousStage"][0].set( previousStage1 );
        game["previousStage"][1].set( previousStage2 );
        previousStage1["nextStage"].set( game );
        previousStage2["nextStage"].set( game );
    maindata->storage.end_critical_operation(lock);

    global_message_update(game,WAIT_SHORT);
    if (old_event!=event)
    {
        global_message_update(old_event,WAIT_SHORT);
        global_message_update(event,WAIT_SHORT);
    }
    if (old_prev_1!=previousStage1)
    {
        global_message_update(old_prev_1,WAIT_SHORT);
        global_message_update(previousStage1,WAIT_SHORT);
    }
    if (old_prev_2!=previousStage2)
    {
        global_message_update(old_prev_2,WAIT_SHORT);
        global_message_update(previousStage2,WAIT_SHORT);
    }
}

void data_edit::set_game_status(Database::Cluster game, int status)
{
    game["gameStatus"].set( status );
    global_message_update(game,WAIT_NOT);
}

void data_edit::set_game_phase(Database::Cluster game, int phase)
{
    game["phase"].set( phase );
    game["gameStatus"].set( GSTATUS_RUNNING );
    global_message_update(game,WAIT_NOT);
}