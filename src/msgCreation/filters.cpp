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
    GameTimeline::FinishIterator it2 = maindata->filters.timeline.running_begin;
    while (it2 != maindata->filters.timeline.running_end) {
        item.put_value((**it2).cluster.index());
        uplist.push_back(std::make_pair("",item));
        ++it2;
    }
    GameTimeline::StartIterator it1 = maindata->filters.timeline.upcoming_begin;
    while (it1 != maindata->filters.timeline.upcoming_end) {
        item.put_value((**it1).cluster.index());
        uplist.push_back(std::make_pair("",item));
        ++it1;
    }
    it2 = maindata->filters.timeline.finished_begin;
    while (it2 != maindata->filters.timeline.finished_end) {
        item.put_value((**it2).cluster.index());
        overlist.push_back(std::make_pair("",item));
        ++it2;
    }

    h2rfp::JSObject out;
    out.put_child("upcoming",uplist);
    out.put_child("over",overlist);
    return out;
}