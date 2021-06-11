#include "game.h"
#include "../misc/response_util.h"
#include "../main-data.h"
#include "../dataEdit/game.h"
#include "../misc/utils.h"
#include "../msgTracking.h"
#include "../tasks/all.h"
#include "../tasks/push.h"

using namespace tobilib;
using namespace h2rfp;

void msg_handler::game_tipp(Session& session, Message& msg)
{
    if (!check_login(session,msg) ||
        !check_parameter(session,msg,"bet1") ||
        !check_parameter(session,msg,"bet2") ||
        !check_parameter(session,msg,"winner") ||
        !check_parameter(session,msg,"topscorer") ||
        !check_parameter(session,msg,"game") )
        return;

    int bet1 = msg.data.get("bet1",0);
    int bet2 = msg.data.get("bet2",0);
    int winnerid = msg.data.get("winner",0);
    int scorerid = msg.data.get("topscorer",0);
    int gameid = msg.data.get("game",0);

    Database::Cluster game = maindata->storage.list("Game")[gameid];
    if (game.is_null()) {
        return_client_error(session,msg,"Unbekanntes Spiel");
        return;
    }
    if (game["gameStatus"].get<int>() != GSTATUS_UPCOMING) {
        return_result(session,msg,make_user_error(ERROR_GAMETIPPS_LOCKED));
        return;
    }
    if (game["teams"][0]->is_null() || game["teams"][1]->is_null()) {
        return_client_error(session,msg,"Die Teams für dieses Spiel sind noch nicht bestimmt");
        return;
    }
    Database::Cluster winner = maindata->storage.list("Team")[winnerid];
    if (winnerid!=0 && winner.is_null()) {
        return_client_error(session,msg,"Team (winner) nicht in der Datenbank");
        return;
    }
    Database::Cluster scorer = maindata->storage.list("Player")[scorerid];
    if (scorerid!=0 && scorer.is_null()) {
        return_client_error(session,msg,"Spieler (topscorer) nicht in der Datenbank");
        return;
    }

    Database::Cluster tipp = data_edit::get_game_tipp(session.user,game,true);
    data_edit::set_game_tipp(tipp,bet1,bet2,winner,scorer);
    
    h2rfp::JSObject answer = make_result();
    answer.put("data.id",tipp.index());
    return_result(session,msg, answer);
}

void msg_handler::game_announce(Session& session, Message& msg)
{
    if (!check_login(session,msg) ||
        !check_permission(session,msg,"perm_gameAnnounce") ||
        !check_parameter(session,msg,"location") ||
        !check_parameter(session,msg,"name") ||
        !check_parameter(session,msg,"stream") ||
        !check_parameter(session,msg,"event") ||
        !check_parameter(session,msg,"team1") ||
        !check_parameter(session,msg,"team2") ||
        !check_parameter(session,msg,"time"))
        return;

    Time starttime = msg.data.get("time",0);
    std::string name = msg.data.get("name","");
    std::string stream = msg.data.get("stream","");
    std::string locname = msg.data.get("location","");

    unsigned int team1id = msg.data.get("team1",0);
    unsigned int team2id = msg.data.get("team2",0);
    unsigned int gameid = msg.data.get("id",0);
    unsigned int previous1id = msg.data.get("previous1",0);
    unsigned int previous2id = msg.data.get("previous2",0);

    Database::Cluster event = maindata->storage.list("Event")[msg.data.get("event",0)];
    Database::Cluster team1 = maindata->storage.list("Team")[team1id];
    Database::Cluster team2 = maindata->storage.list("Team")[team2id];
    Database::Cluster previous1 = maindata->storage.list("Game")[previous1id];
    Database::Cluster previous2 = maindata->storage.list("Game")[previous2id];

    if (event.is_null())
    {
        return_client_error(session,msg,"Das Event existiert nicht");
        return;
    }
    if (team1id!=0) {
        if (team1.is_null()) {
            return_client_error(session,msg,"team1 existiert nicht");
            return;
        }
        if (!list_contains_cluster(event["teams"],team1)) {
            return_client_error(session,msg,"Das Team1 nimmt nicht an diesem Event teil");
            return;
        }
    }
    if (team2id!=0) {
        if (team2.is_null()) {
            return_client_error(session,msg,"team2 existiert nicht");
            return;
        }
        if (!list_contains_cluster(event["teams"],team2)) {
            return_client_error(session,msg,"Das Team2 nimmt nicht an diesem Event teil");
            return;
        }
    }
    if (previous1id != 0) {
        if (previous1.is_null()) {
            return_client_error(session,msg,"previous1 existiert nicht");
            return;
        }
        if (!list_contains_cluster(event["teams"],previous1)) {
            return_client_error(session,msg,"Die Vorstufe1 gehört nicht zum selben Anlass");
            return;
        }
        if (!previous1["nextStage"]->is_null() && previous1["nextStage"]->index()!=gameid) {
            return_client_error(session,msg,"prevous1 wird bereits als vorstufe verwendet");
            return;
        }
    }
    if (previous2id != 0) {
        if (previous2.is_null()) {
            return_client_error(session,msg,"previous2 existiert nicht");
            return;
        }
        if (!list_contains_cluster(event["teams"],previous2)) {
            return_client_error(session,msg,"Die Vorstufe2 gehört nicht zum selben Anlass");
            return;
        }
        if (!previous2["nextStage"]->is_null() && previous2["nextStage"]->index()!=gameid) {
            return_client_error(session,msg,"prevous2 wird bereits als vorstufe verwendet");
            return;
        }
    }
    
    if (previous1id * team1id != 0) {
        return_client_error(session,msg,"Team1 wird bereits durch previous1 gesteuert");
        return;
    }
    if (previous2id * team2id != 0) {
        return_client_error(session,msg,"Team2 wird bereits durch previous2 gesteuert");
        return;
    }
    
    Database::Cluster game;
    if (gameid != 0)
    {
        game = maindata->storage.list("Game")[gameid];
        if (game.is_null()) {
            return_client_error(session,msg,"Das Spiel existiert nicht");
            return;
        }
        if (game["gameStatus"].get<int>() == GSTATUS_ENDED) {
            return_client_error(session,msg,"Dieses Spiel ist beendet und kann nicht bearbeitet werden.");
            return;
        }
        if (game["tipps"].begin() != game["tipps"].end()) {
            return_client_error(session,msg,"Auf dieses Spiel wurde schon getippt. Es kann nicht mehr bearbeitet werden");
            return;
        }
    }
    else
    {
        game = data_edit::create_game();
    }

    data_edit::announce_game(
        session.user,
        game,
        starttime,
        data_edit::get_location(session.user,locname),
        event,
        name,
        stream,
        team1,
        team2,
        previous1,
        previous2
    );

    h2rfp::JSObject answer = make_result();
    answer.put("data.id", game.index());
    return_result(session,msg,answer);
    return;
}

void msg_handler::game_shift_phase(Session& session, Message& msg)
{
    if (!check_login(session,msg)
        || !check_permission(session,msg,"perm_liveReport")
        || !check_parameter(session,msg,"game"))
        return;
    
    Database::Cluster game = maindata->storage.list("Game")[msg.data.get("game",0)];
    int phase = game["phase"].get<int>();
    if (game["gameStatus"].get<int>() != GSTATUS_PENDING || game["phase"].get<int>() >= GAMEPHASE_PENALTY)
    {
        return_client_error(session,msg,"Die Phase dieses Spiels kann gerade nicht bearbeitet werden.");
        return;
    }

    data_edit::set_game_phase(game,++phase);

    return_result(session,msg,make_result());
}

void msg_handler::game_goal(Session& session, Message& msg)
{
    if (!check_login(session,msg)
        || !check_permission(session,msg,"perm_liveReport")
        || !check_parameter(session,msg,"game")
        || !check_parameter(session,msg,"player")
        || !check_parameter(session,msg,"team")
        || !check_parameter(session,msg,"penalty"))
        return;

    Database::Cluster game = maindata->storage.list("Game")[msg.data.get("game",0)];
    Database::Cluster player = maindata->storage.list("Player")[msg.data.get("player",0)];
    Database::Cluster team = maindata->storage.list("Team")[msg.data.get("team",0)];
    bool penalty = msg.data.get("penalty",false);

    if (game.is_null())
    {
        return_client_error(session,msg,"Das spiel wurde nicht gefunden");
        return;
    }
    if (game["gameStatus"].get<int>() != GSTATUS_RUNNING && game["gameStatus"].get<int>() != GSTATUS_PENDING)
    {
        return_client_error(session,msg,"Das Spiel ist nicht am laufen");
        return;
    }
    if (player.is_null())
    {
        return_client_error(session,msg,"Der spieler wurde nicht gefunden");
        return;
    }
    if (team.is_null())
    {
        return_client_error(session,msg,"Das Team wurde nicht gefunden");
        return;
    }

    // HINWEIS
    // Die folgenden Zeilen sollten eigentlich über data_edit gehen. Aber scheiss drauf

    Database::Member scorecount;
    if (*game["teams"][0] == team) {
        scorecount = penalty ? game["penalty"][0] : game["scores"][0];
    }
    else if (*game["teams"][1] == team) {
        scorecount = penalty ? game["penalty"][1] : game["scores"][1];
    }
    else {
        return_client_error(session,msg,"Das team nimmt nicht am spiel teil");
        return;
    }

    scorecount.set( scorecount.get<int>() + 1);
    if (!penalty)
        game["scorers"].emplace().set( player );


    global_message_update(game,WAIT_NOT);
    return_result(session,msg,make_result());
}

void msg_handler::game_report(Session& session, Message& msg)
{
    if (!check_login(session,msg) ||
        !check_permission(session,msg,"perm_gameReport") ||
        !check_parameter(session,msg,"game") ||
        !check_parameter(session,msg,"phase") ||
        !check_parameter(session,msg,"score1") ||
        !check_parameter(session,msg,"score2") ||
        !check_parameter(session,msg,"scorePenalty1") ||
        !check_parameter(session,msg,"scorePenalty2") ||
        !check_parameter(session,msg,"scorers") )
        return;
    
    Database::Cluster game = maindata->storage.list("Game")[msg.data.get("game",0)];
    if (game.is_null()) {
        return_client_error(session,msg,"Das Spiel existiert nicht");
        return;
    }
    if (game["gameStatus"].get<int>() < GSTATUS_PENDING) {
        return_client_error(session,msg,"Dieses Spiel ist noch am laufen");
        return;
    }

    int phase = msg.data.get("phase",0);
    int score1 = msg.data.get("score1",0);
    int score2 = msg.data.get("score2",0);
    int penalty1 = msg.data.get("scorePenalty1",0);
    int penalty2 = msg.data.get("scorePenalty2",0);
    std::vector<Database::Cluster> scorers;

    for (auto& item: msg.data.get_child("scorers")) {
        Database::Cluster scorer = maindata->storage.list("Player")[item.second.get_value(0)];
        if (scorer.is_null()) {
            return_client_error(session,msg,"Ein Spieler ist unbekannt");
            return;
        }
        scorers.push_back(scorer);
    }

    data_edit::report_game(session.user,game,phase,score1,score2,penalty1,penalty2,scorers);
    maindata->tasks.push.game_results( game );

    return_result(session,msg,make_result());
}