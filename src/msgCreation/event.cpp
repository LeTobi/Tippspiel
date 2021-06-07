#include "event.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::event_is_individual(Session& session, const MsgID& id)
{
    return false;
}

JSObject msg_creation::detail::event_make_msg(Session& session, const MsgID& id)
{
    Database::Cluster event = maindata->storage.list("Event")[id.arg0];
    h2rfp::JSObject out;
    if (event.is_null())
        return out;
    out.put("id",event.index());
    out.put("name",event["name"].get<std::string>());
    out.put("description", event["description"].get<std::string>());
    out.put("deadline", event["deadline"].get<int>());
    out.put("status",event["eventStatus"].get<int>());
    h2rfp::JSObject list;
    h2rfp::JSObject item;
    for (auto game: event["games"])
    {
        item.put_value(game->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("games",list);
    list.clear();
    for (auto team: event["teams"])
    {
        item.put_value(team->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("teams",list);
    list.clear();
    for (auto rank: event["ranks"])
    {
        if (rank["eventtipp"]->is_null())
            continue;
        item.put_value(rank["eventtipp"]->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("tipps",list);
    out.put("winner",event["winner"]->index());
    out.put("topscorer",event["topscorer"]->index());
    return out;
}