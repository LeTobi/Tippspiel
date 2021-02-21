#include "game.h"
#include "../session/utils.h"

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
    if (name.size() < 5) {
        h2rfp::JSObject result = make_user_error(ERROR_INVALID_NAME);
        result.put("data.name",name);
        return_result(client,msg,result);
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
    
    return_result(client,msg, make_result());
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