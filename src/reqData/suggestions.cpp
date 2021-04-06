#include "suggestions.h"
#include "../misc/response_util.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

void msg_handler::suggest_locations(Session& session, const Message& msg)
{
    JSObject answer = make_result();
    answer.put_child("data", maindata->cache.get_data(session,MsgType::locations));
    return_result(session,msg,answer);
}

void msg_handler::suggest_players(Session& session, const Message& msg)
{
    if (!check_parameter(session,msg,"game"))
        return;
    Database::Cluster game = maindata->storage.list("Game")[msg.data.get("game",0)];
    if (game.is_null()) {
        return_client_error(session,msg,"Das Spiel existiert nicht");
        return;
    }

    JSObject answer = make_result();
    answer.put_child(
        "data",
        maindata->cache.get_data(
            session,
            MsgID(MsgType::gamePlayers,game.index())
            )
        );
    return_result(session,msg,answer);
}