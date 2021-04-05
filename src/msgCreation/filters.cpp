#include "filters.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;
using namespace filters;

bool msg_creation::detail::hot_games_individual(Session&, const MsgID&) {
    return false;
}

JSObject msg_creation::detail::hot_games(Session&, const MsgID&) {
    GameTimeline::Iterator top = maindata->gametimeline.future_horizon;
    GameTimeline::Iterator center = maindata->gametimeline.most_current;
    GameTimeline::Iterator bottom = maindata->gametimeline.past_horizon; 
    h2rfp::JSObject uplist;
    h2rfp::JSObject overlist;
    h2rfp::JSObject item;
    while (bottom!=center) {
        item.put_value((**bottom).cluster.index());
        overlist.push_back(std::make_pair("",item));
        ++bottom;
    }
    while (center!=top) {
        item.put_value((**center).cluster.index());
        uplist.push_back(std::make_pair("",item));
        ++center;
    }

    h2rfp::JSObject out;
    out.put_child("upcoming",uplist);
    out.put_child("over",overlist);
    return out;
}