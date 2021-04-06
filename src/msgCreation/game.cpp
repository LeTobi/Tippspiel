#include "game.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::game_is_individual(Session& session, const MsgID& id)
{
    return false;
}

JSObject msg_creation::detail::game_make_msg(Session& session, const MsgID& id)
{
    Database::Cluster game = maindata->storage.list("Game")[id.arg0];
    h2rfp::JSObject out;
    if (game.is_null())
        return out;
    out.put("id",game.index());
    out.put("start",game["start"].get<int>());
    out.put("location",game["location"]["name"].get<std::string>());
    out.put("stream",game["stream"].get<std::string>());
    out.put("event",game["event"]->index());
    out.put("status",game["gameStatus"].get<int>());
    out.put("phase",game["phase"].get<int>());
    out.put("team1",game["teams"][0]->index());
    out.put("team2",game["teams"][1]->index());
    out.put("score1",game["scores"][0].get<int>());
    out.put("score2",game["scores"][1].get<int>());
    out.put("penaltyScore1",game["penalty"][0].get<int>());
    out.put("penaltyScore2",game["penalty"][1].get<int>());
    h2rfp::JSObject list;
    for (auto scorer: game["scorers"])
    {
        h2rfp::JSObject item;
        item.put_value(scorer->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("scorers",list);
    list.clear();
    for (auto tipp: game["tipps"])
    {
        h2rfp::JSObject item;
        item.put_value(tipp->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("tipps",list);

    // not synced:
    out.put("short1", game["teams"][0]["short"].get<std::string>());
    out.put("short2", game["teams"][1]["short"].get<std::string>());
    return out;
}

bool msg_creation::detail::suggest_players_individual(Session&, const MsgID&)
{
    return false;
}

JSObject msg_creation::detail::suggest_players(Session& session, const MsgID& msgid)
{
    Database::Cluster game = maindata->storage.list("Game")[msgid.arg0];
    h2rfp::JSObject names;
    h2rfp::JSObject item;
    for (auto team: game["teams"])
        for (auto player: team["players"])
        {
            item.put_value(player->index());
            names.push_back(std::make_pair("",item));
        }
    return names;
}