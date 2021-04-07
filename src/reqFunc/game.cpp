#include "game.h"
#include "../misc/response_util.h"
#include "../main-data.h"
#include "../dataEdit/game.h"

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
        if (*team1["event"] != event) {
            return_client_error(session,msg,"Das Team1 nimmt nicht an diesem Event teil");
            return;
        }
    }
    if (team2id!=0) {
        if (team2.is_null()) {
            return_client_error(session,msg,"team2 existiert nicht");
            return;
        }
        if (*team2["event"] != event) {
            return_client_error(session,msg,"Das Team2 nimmt nicht an diesem Event teil");
            return;
        }
    }
    if (previous1id != 0) {
        if (previous1.is_null()) {
            return_client_error(session,msg,"previous1 existiert nicht");
            return;
        }
        if (*previous1["event"] != event) {
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
        if (*previous2["event"] != event) {
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