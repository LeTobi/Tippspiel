#include "identity.h"
#include "../misc/response_util.h"
#include "../misc/msgID.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

void msg_handler::inform_identity(Session& session, const Message& msg)
{
    if (!check_login(session,msg))
        return;

    h2rfp::JSObject gtipps;
    h2rfp::JSObject etipps;
    h2rfp::JSObject groups;
    h2rfp::JSObject item;
    for (auto gtipp: session.user["gameTipps"]) {
        item.put_value(gtipp->index());
        gtipps.push_back(std::make_pair("", item));
    }
    for (auto etipp: session.user["eventTipps"]) {
        item.put_value(etipp->index());
        etipps.push_back(std::make_pair("", item));
    }
    for (auto group: session.user["groups"]) {
        item.put_value(group->index());
        groups.push_back(std::make_pair("", item));
    }

    h2rfp::JSObject answer = make_result();
    answer.put("data.id", session.user.index());
    answer.put("data.name", session.user["name"].get<std::string>());
    answer.add_child("data.eventTipps",etipps);
    answer.add_child("data.gameTipps",gtipps);
    answer.add_child("data.groups",gtipps);
    answer.put("data.points", session.user["points"].get<int>());
    answer.put("data.permission.eventAnnounce", session.user["perm_eventAnnounce"].get<bool>());
    answer.put("data.permission.eventReport", session.user["perm_eventReport"].get<bool>());
    answer.put("data.permission.gameAnnounce", session.user["perm_gameAnnounce"].get<bool>());
    answer.put("data.permission.gameReport", session.user["perm_gameReport"].get<bool>());
    answer.put("data.permission.console", session.user["perm_console"].get<bool>());
    session.client.respond(msg.id, answer);
}