#include "player.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::player_is_individual(Session& session, const MsgID& id)
{
    return false;
}

JSObject msg_creation::detail::player_make_msg(Session& session, const MsgID& id)
{
    Database::Cluster player = maindata->storage.list("Player")[id.arg0];
    h2rfp::JSObject out;
    if (player.is_null())
        return out;
    out.put("id",player.index());
    out.put("name",player["name"].get<std::string>());
    out.put("team",player["team"]->index());
    return out;
}