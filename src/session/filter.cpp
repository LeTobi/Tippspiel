#include "filter.h"
#include "utils.h"

void filter_hot_games(Client& client, h2rfp::Message msg)
{
    std::vector<Database::Cluster> upcoming = data->observer.get_upcoming();
    std::vector<Database::Cluster> finished = data->observer.get_finished();
    h2rfp::JSObject uplist;
    h2rfp::JSObject overlist;
    h2rfp::JSObject item;
    for (auto& game: upcoming) {
        item.put_value(game.index());
        uplist.push_back(std::make_pair("",item));
    }
    for (auto it = finished.rbegin(); it!=finished.rend(); ++it) {
        item.put_value(it->index());
        overlist.push_back(std::make_pair("",item));
    }

    h2rfp::JSObject out = make_result();
    out.put_child("data.upcoming",uplist);
    out.put_child("data.over",overlist);
    return_result(client,msg,out);
}

void suggest_players(Client& client, h2rfp::Message msg)
{
    if (!check_parameter(client,msg,"game"))
        return;
    Database::Cluster game = data->database.list("Game")[msg.data.get("game",0)];
    if (game.is_null()) {
        return_client_error(client,msg,"Das Spiel existiert nicht");
        return;
    }
    h2rfp::JSObject names;
    h2rfp::JSObject item;
    for (auto team: game["teams"])
        for (auto player: team["players"])
        {
            item.put_value(player->index());
            names.push_back(std::make_pair("",item));
        }
    h2rfp::JSObject out = make_result();
    out.put_child("data",names);
    return_result(client,msg,out);
}