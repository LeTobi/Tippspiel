#include "team.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::team_is_individual(Session& session, const MsgID& id)
{
    return false;
}

JSObject msg_creation::detail::team_make_msg(Session& session, const MsgID& id)
{
    Database::Cluster team = maindata->storage.list("Team")[id.arg0];
    h2rfp::JSObject out;
    if (team.is_null())
        return out;
    out.put("id",team.index());
    out.put("name",team["name"].get<std::string>());
    out.put("short", team["short"].get<std::string>());
    h2rfp::JSObject list;
    h2rfp::JSObject item;
    for (auto player: team["players"])
    {
        item.put_value(player->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("players",list);
    return out;
}