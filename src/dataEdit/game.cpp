#include "game.h"
#include "../main-data.h"
#include "../msgTracking.h"
#include "../misc/enums.h"
#include "points.h"

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
        Database::Cluster rank = get_rank(user, *game["event"], true);
        rank["tippcount"].set( rank["tippcount"].get<int>() + 1 );
        game["tippcount"].set( game["tippcount"].get<int>() + 1 );
        game["tipps"].emplace().set( tipp );
        rank["gametipps"].emplace().set( tipp );
        tipp["user"].set( user );
        tipp["game"].set( game );
        tipp["rank"].set( rank );
    maindata->storage.end_critical_operation(lock);

    global_message_update(tipp, WAIT_LONG);
    global_message_update(user, WAIT_LONG);
    global_message_update(rank, WAIT_LONG);
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

void data_edit::delete_game(Database::Cluster game)
{
    FlagRequest lock = maindata->storage.begin_critical_operation();
        Database::Cluster event = *game["event"];
        game["event"].set( Database::Cluster() );
        event["games"].erase( game );
        Database::Member tipps = game["tipps"];
        while (tipps.begin() != tipps.end())
            delete_game_tipp(**tipps.begin());
        if (*game["nextStage"]["previousStage"][0] == game)
            game["nextStage"]["previousStage"][0].set( Database::Cluster() );
        if (*game["nextStage"]["previousStage"][1] == game)
            game["nextStage"]["previousStage"][1].set( Database::Cluster() );
        game["nextStage"].set( Database::Cluster() );
        game["previousStage"][0]["nextStage"].set( Database::Cluster() );
        game["previousStage"][1]["nextStage"].set( Database::Cluster() );
        game["previousStage"][0].set( Database::Cluster() );
        game["previousStage"][1].set( Database::Cluster() );

        global_message_delete(game);
        game.erase();
    maindata->storage.end_critical_operation(lock);

    global_message_update(event,WAIT_NOT);
}

void data_edit::delete_game_tipp(Database::Cluster gametipp)
{
    FlagRequest lock = maindata->storage.begin_critical_operation();
        Database::Cluster game = *gametipp["game"];
        Database::Cluster rank = *gametipp["rank"];

        gametipp["game"]["tipps"].erase( gametipp );
        gametipp["game"]["tippcount"].set( gametipp["game"]["tippcount"].get<int>() - 1);
        gametipp["game"].set( Database::Cluster() );
        int reward = gametipp["reward"].get<int>();
        gametipp["rank"]["points"].set( gametipp["rank"]["points"].get<int>() - reward );
        gametipp["rank"]["tippcount"].set( gametipp["rank"]["tippcount"].get<int>() - 1);
        gametipp["rank"]["gametipps"].erase( gametipp );
        gametipp["rank"].set( Database::Cluster() );

        global_message_delete(gametipp);
        gametipp.erase();
    maindata->storage.end_critical_operation(lock);

    global_message_update(game, WAIT_NOT);
    global_message_update(rank, WAIT_NOT);
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

void data_edit::report_game(
        tobilib::Database::Cluster user,
        tobilib::Database::Cluster game,
        int phase,
        int score1,
        int score2,
        int penalty1,
        int penalty2,
        const std::vector<tobilib::Database::Cluster>& scorers
        )
{
    game["reporter"].set( user );
    game["reportedAt"].set( get_time() );
    game["gameStatus"].set( GSTATUS_ENDED );
    game["phase"].set( phase );
    game["scores"][0].set( score1 );
    game["scores"][1].set( score2 );
    game["penalty"][0].set( penalty1 );
    game["penalty"][1].set( penalty2 );

    Database::Member db_scorers = game["scorers"];
    while (db_scorers.begin() != db_scorers.end())
        db_scorers.erase(db_scorers.begin());
    for (tobilib::Database::Cluster scorer: scorers)
        db_scorers.emplace().set( scorer );

    game_evaluate(game);
    global_message_update(game,WAIT_SHORT);
}

void data_edit::game_evaluate(Database::Cluster game)
{
    int phase = game["phase"].get<int>();
    int score1 = game["scores"][0].get<int>();
    int score2 = game["scores"][1].get<int>();
    int penalty1 = game["penalty"][0].get<int>();
    int penalty2 = game["penalty"][1].get<int>();
    Database::Member db_scorers = game["scorers"];
    Database::Member db_tipps = game["tipps"];
    bool is_penalty = score1==score2;

    Database::Cluster winnerteam;
    if (score1>score2)
        winnerteam = *game["teams"][0];
    else if (score2>score1)
        winnerteam = *game["teams"][1];
    else if (phase>=GAMEPHASE_PENALTY) {
        if (penalty1>penalty2)
            winnerteam = *game["teams"][0];
        if (penalty2<penalty1)
            winnerteam = *game["teams"][1];
    }

    struct TippResult {
        bool team;
        bool diff;
        bool exact;
        bool draw;
        int goals = 0;
    };
    
    std::vector<TippResult> results;
    for (Database::Member tipp: db_tipps)
    {
        TippResult res;
        int tscore1 = tipp["bet"][0].get<int>();
        int tscore2 = tipp["bet"][1].get<int>();
        Database::Cluster twinner = *tipp["winner"];
        Database::Cluster tscorer = *tipp["topscorer"];
        bool t_is_penalty = tscore1==tscore2;

        res.team = (!is_penalty && !t_is_penalty && (twinner == winnerteam)) || (is_penalty && t_is_penalty);
        res.diff = score1-score2 == tscore1-tscore2;
        res.exact = score1==tscore1 && score2==tscore2;
        res.draw = phase >= GAMEPHASE_PENALTY && is_penalty && t_is_penalty;
        for (Database::Member scorer: db_scorers)
            if (*scorer==tscorer)
                res.goals++;
        results.push_back(res);
    }

    FlagRequest lock = maindata->storage.begin_critical_operation();
        game["winner"].set( winnerteam );
        auto it = results.begin();
        for (Database::Member tippptr: db_tipps) {
            tipp_set_result(*tippptr,it->team,it->diff,it->exact,it->draw,it->goals);
            ++it;
        }
        game_fetch_stage(*game["nextStage"]);
    maindata->storage.end_critical_operation(lock);

    global_message_update(game,WAIT_NOT);
}

void data_edit::game_fetch_stage(Database::Cluster game)
{
    if (game.is_null())
        return;
    Database::Cluster team1before = *game["teams"][0];
    Database::Cluster team1after = team1before;
    Database::Cluster team2before = *game["teams"][1];
    Database::Cluster team2after = team2before;
    Database::Cluster winnerbefore = *game["winner"];
    Database::Cluster winnerafter = winnerbefore;
    if (!game["previousStage"][0]->is_null())
        team1after = *game["previousStage"][0]["winner"];
    if (!game["previousStage"][1]->is_null())
        team2after = *game["previousStage"][1]["winner"];

    if (winnerbefore == team1before)
        winnerafter = team1after;
    if (winnerbefore == team2before)
        winnerafter = team2after;

    game["teams"][0].set( team1after );
    game["teams"][1].set( team2after );
    game["winner"].set( winnerafter );

    if (winnerbefore != winnerafter)
    {
        game_fetch_stage(*game["nextStage"]);
        global_message_update(game, WAIT_SHORT);
    }
    else if (team1before != team1after || team2before != team2after)
    {
        global_message_update(game, WAIT_SHORT);
    }
}