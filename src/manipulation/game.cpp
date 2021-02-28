#include "game.h"
#include "../session/utils.h"
#include "utils.h"

Database::Cluster get_tipp(Database::Cluster user, Database::Cluster game)
{
    for (auto tipp: game["tipps"])
    {
        if (*tipp["user"] == user)
            return *tipp;
    }
    FlagRequest lock = data->database.begin_critical_operation();
        Database::Cluster tipp = data->database.list("GameTipp").emplace();
        user["gameTipps"].emplace().set( tipp );
        game["tipps"].emplace().set( tipp );
        tipp["user"].set( user );
        tipp["game"].set( game );
    data->database.end_critical_operation(lock);

    data->virtualdb.update(game, VirtualDB::WAIT_SHORT);
    data->virtualdb.update(user, VirtualDB::WAIT_LONG);

    return tipp;
}

void create_player(Client& client, h2rfp::Message msg)
{
    if (!check_login(client,msg) ||
        !check_parameter(client,msg,"name") ||
        !check_parameter(client,msg,"team"))
        return;
    Database::Cluster team = data->database.list("Team")[msg.data.get("team",0)];
    std::string name = msg.data.get("name","");
    if (team.is_null()) {
        return_client_error(client,msg,"Das Team ist unbekannt");
        return;
    }
    if (name.size() < 4) {
        h2rfp::JSObject result = make_user_error(ERROR_INVALID_NAME);
        result.put("data.name",name);
        return_result(client,msg,result);
        return;
    }
    
    FlagRequest lock = data->database.begin_critical_operation();
        Database::Cluster player = data->database.list("Player").emplace();
        player["creator"].set( client.user );
        player["createdAt"].set ( get_time() );
        player["name"].set( name );
        player["team"].set( team );
        team["players"].emplace().set( player );
    data->database.end_critical_operation(lock);

    data->virtualdb.update(player, VirtualDB::WAIT_SHORT);
    data->virtualdb.update(team, VirtualDB::WAIT_SHORT);

    h2rfp::JSObject result = make_result();
    result.put("data.id", player.index());
    return_result(client,msg,result);
    return;
}

void make_tipp(Client& client, h2rfp::Message msg)
{
    if (!check_login(client,msg) ||
        !check_parameter(client,msg,"bet1") ||
        !check_parameter(client,msg,"bet2") ||
        !check_parameter(client,msg,"winner") ||
        !check_parameter(client,msg,"topscorer") ||
        !check_parameter(client,msg,"game") )
        return;

    int bet1 = msg.data.get("bet1",0);
    int bet2 = msg.data.get("bet2",0);
    int winnerid = msg.data.get("winner",0);
    int scorerid = msg.data.get("topscorer",0);
    int gameid = msg.data.get("game",0);

    Database::Cluster game = data->database.list("Game")[gameid];
    if (game.is_null()) {
        return_client_error(client,msg,"Unbekanntes Spiel");
        return;
    }
    if (game["teams"][0]->is_null() || game["teams"][1]->is_null()) {
        return_client_error(client,msg,"Die Teams für dieses Spiel sind noch nicht bestimmt");
        return;
    }
    Database::Cluster winner = data->database.list("Team")[winnerid];
    if (winnerid!=0 && winner.is_null()) {
        return_client_error(client,msg,"Team (winner) nicht in der Datenbank");
        return;
    }
    Database::Cluster scorer = data->database.list("Player")[scorerid];
    if (scorerid!=0 && scorer.is_null()) {
        return_client_error(client,msg,"Spieler (topscorer) nicht in der Datenbank");
        return;
    }

    Database::Cluster tipp = get_tipp(client.user,game);
    tipp["createdAt"].set( get_time() );
    tipp["bet"][0].set( bet1 );
    tipp["bet"][1].set( bet2 );
    tipp["winner"].set( winner );
    tipp["topscorer"].set( scorer );
    
    // update tipp nicht nötig, da nicht sichtbar
    
    h2rfp::JSObject answer = make_result();
    answer.put("data.id",tipp.index());
    return_result(client,msg, answer);
}

void create_game(Client& client, h2rfp::Message msg)
{
    if (!check_login(client,msg) ||
        !check_permission(client,msg,"perm_gameAnnounce") ||
        !check_parameter(client,msg,"location") ||
        !check_parameter(client,msg,"name") ||
        !check_parameter(client,msg,"stream") ||
        !check_parameter(client,msg,"event") ||
        !check_parameter(client,msg,"team1") ||
        !check_parameter(client,msg,"team2"))
        return;

    unsigned int team1id = msg.data.get("team1",0);
    unsigned int team2id = msg.data.get("team2",0);
    unsigned int gameid = msg.data.get("id",0);
    unsigned int previous1id = msg.data.get("previous1",0);
    unsigned int previous2id = msg.data.get("previous2",0);

    Database::Cluster event = data->database.list("Event")[msg.data.get("event",0)];
    Database::Cluster team1 = data->database.list("Team")[team1id];
    Database::Cluster team2 = data->database.list("Team")[team2id];
    Database::Cluster previous1 = data->database.list("Game")[previous1id];
    Database::Cluster previous2 = data->database.list("Game")[previous2id];

    if (event.is_null())
    {
        return_client_error(client,msg,"Das Event existiert nicht");
        return;
    }
    if (team1id!=0) {
        if (team1.is_null()) {
            return_client_error(client,msg,"team1 existiert nicht");
            return;
        }
        if (*team1["event"] != event) {
            return_client_error(client,msg,"Das Team1 nimmt nicht an diesem Event teil");
            return;
        }
    }
    if (team2id!=0) {
        if (team2.is_null()) {
            return_client_error(client,msg,"team2 existiert nicht");
            return;
        }
        if (*team2["event"] != event) {
            return_client_error(client,msg,"Das Team2 nimmt nicht an diesem Event teil");
            return;
        }
    }
    if (previous1id != 0) {
        if (previous1.is_null()) {
            return_client_error(client,msg,"previous1 existiert nicht");
            return;
        }
        if (*previous1["event"] != event) {
            return_client_error(client,msg,"Die Vorstufe1 gehört nicht zum selben Anlass");
            return;
        }
        if (!previous1["nextStage"]->is_null() && previous1["nextStage"]->index()!=gameid) {
            return_client_error(client,msg,"prevous1 wird bereits als vorstufe verwendet");
            return;
        }
    }
    if (previous2id != 0) {
        if (previous2.is_null()) {
            return_client_error(client,msg,"previous2 existiert nicht");
            return;
        }
        if (*previous2["event"] != event) {
            return_client_error(client,msg,"Die Vorstufe2 gehört nicht zum selben Anlass");
            return;
        }
        if (!previous2["nextStage"]->is_null() && previous2["nextStage"]->index()!=gameid) {
            return_client_error(client,msg,"prevous2 wird bereits als vorstufe verwendet");
            return;
        }
    }
    
    if (previous1id * team1id != 0) {
        return_client_error(client,msg,"Team1 wird bereits durch previous1 gesteuert");
        return;
    }
    if (previous2id * team2id != 0) {
        return_client_error(client,msg,"Team2 wird bereits durch previous2 gesteuert");
        return;
    }
    
    Database::Cluster game;
    if (gameid != 0)
    {
        game = data->database.list("Game")[gameid];
        if (game.is_null()) {
            return_client_error(client,msg,"Das Spiel existiert nicht");
            return;
        }
        if (game["gameStatus"].get<int>() == GSTATUS_ENDED) {
            return_client_error(client,msg,"Dieses Spiel ist beendet und kann nicht bearbeitet werden.");
            return;
        }
        if (game["tipps"].begin() != game["tipps"].end()) {
            return_client_error(client,msg,"Auf dieses Spiel wurde schon getippt. Es kann nicht mehr bearbeitet werden");
            return;
        }
    }

    FlagRequest lock = data->database.begin_critical_operation();
        if (game.is_null()) {
            game = data->database.list("Game").emplace();
            game["creator"].set( client.user );
            game["createdAt"].set( get_time() );
        }
        game["start"].set( msg.data.get("time",0) );
        game["location"].set( get_location(client.user, msg.data.get("location","")) );
        game["name"].set( msg.data.get("name","") );
        game["stream"].set( msg.data.get("stream","") );
        if (game["event"]->is_null()) {
            game["event"].set( event );
            event["games"].emplace().set( game );
        }
        game["phase"].set( GAMEPHASE_NORMAL );
        game["teams"][0].set( team1 );
        game["teams"][1].set( team2 );
        game["previousStage"][0].set( previous1 );
        game["previousStage"][1].set( previous2 );
        previous1["nextStage"].set( game );
        previous2["nextStage"].set( game );
    data->database.end_critical_operation(lock);

    data->virtualdb.update(game, VirtualDB::WAIT_SHORT);
    data->virtualdb.update(event, VirtualDB::WAIT_SHORT);

    h2rfp::JSObject answer = make_result();
    answer.put("data.id", game.index());
    return_result(client,msg,answer);
    return;
}

void report_game(Client& client, h2rfp::Message msg)
{
    if (!check_login(client,msg) ||
        !check_permission(client,msg,"perm_gameReport") ||
        !check_parameter(client,msg,"game") ||
        !check_parameter(client,msg,"score1") ||
        !check_parameter(client,msg,"score2") ||
        !check_parameter(client,msg,"scorePenalty1") ||
        !check_parameter(client,msg,"scorePenalty2") ||
        !check_parameter(client,msg,"scorers") )
        return;
    
    return_server_error(client,msg,"Diese Funktion ist nicht fertig implementiert");
    return;

    Database::Cluster game = data->database.list("Game")[msg.data.get("game",0)];
    if (game.is_null()) {
        return_client_error(client,msg,"Das Spiel existiert nicht");
        return;
    }
    if (game["start"].get<int>() > get_time()) {
        return_client_error(client,msg,"Dieses Spiel hat noch nicht mal angefangen! O.o");
        return;
    }

    int score1 = msg.data.get("score1",0);
    int score2 = msg.data.get("score2",0);
    int penalty1 = msg.data.get("scorePenalty1",0);
    int penalty2 = msg.data.get("scorePenalty2",0);
    game["scores"][0].set( score1 );
    game["scores"][1].set( score2 );
    game["penalty"][0].set( penalty1 );
    game["penalty"][1].set( penalty2 );
    game["gameStatus"].set( GSTATUS_ENDED );

    while (game["scorers"].begin() != game["scorers"].end())
        game["scorers"].erase(game["scorers"].begin());
    
    for (auto& item: msg.data.get_child("scorers")) {
        Database::Cluster scorer = data->database.list("Player")[item.second.get_value(0)];
        if (scorer.is_null()) {
            return_client_error(client,msg,"Ein Spieler ist unbekannt");
            return;
        }
        game["scorers"].emplace().set( scorer );
    }


    if (score1 > score2)
        game["winner"].set( *game["teams"][0] );
    else if (score2 > score1)
        game["winner"].set( *game["teams"][1] );
    else if (penalty1 > penalty2)
        game["winner"].set( *game["teams"][0] );
    else if (penalty2 > penalty1)
        game["winner"].set( *game["teams"][1] );
    else
        game["winner"].set( Database::Cluster() );

    // TODO punkte für sieger

    FlagRequest lock = data->database.begin_critical_operation();
        for(auto tipp: game["tipps"])
        {
            int userscore = tipp["user"]["points"].get<int>();
            int tippreward = tipp["reward"].get<int>();
            userscore -= tippreward;
            tippreward = 0;
            Database::Cluster playertipp = *tipp["topscorer"];
            for (auto player: game["scorers"])
                if (playertipp == *player)
                    tippreward++;
            userscore += tippreward;
            tipp["user"]["points"].set( userscore );
            tipp["reward"].set( tippreward );

            data->virtualdb.update(*tipp, VirtualDB::WAIT_SHORT);
            data->virtualdb.update(*tipp["user"], VirtualDB::WAIT_SHORT);
        }
    data->database.end_critical_operation(lock);

    data->virtualdb.update(game, VirtualDB::WAIT_NOT);
    return_result(client,msg,make_result());
    return;
}