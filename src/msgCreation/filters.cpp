#include "filters.h"
#include "../main-data.h"
#include "../filters/all.h"
#include "../filters/gameTimeline.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::hot_games_individual(Session&, const MsgID&) {
    return false;
}

JSObject msg_creation::detail::hot_games(Session&, const MsgID&) {
    h2rfp::JSObject uplist;
    h2rfp::JSObject overlist;
    h2rfp::JSObject item;
    for (GameTimeline::Game* game: maindata->filters.timeline.pending) {
        item.put_value(game->cluster.index());
        uplist.push_back(std::make_pair("",item));
    }
    for (GameTimeline::Game* game: maindata->filters.timeline.running) {
        item.put_value(game->cluster.index());
        uplist.push_back(std::make_pair("",item));
    }
    for (GameTimeline::Game* game: maindata->filters.timeline.upcoming) {
        item.put_value(game->cluster.index());
        uplist.push_back(std::make_pair("",item));
    }
    for (GameTimeline::Game* game: maindata->filters.timeline.finished) {
        item.put_value(game->cluster.index());
        overlist.push_back(std::make_pair("",item));
    }

    h2rfp::JSObject out;
    out.put_child("upcoming",uplist);
    out.put_child("over",overlist);
    return out;
}