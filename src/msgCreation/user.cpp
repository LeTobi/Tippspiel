#include "user.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::user_is_individual(Session& session, const MsgID& id)
{
    return id.arg0 == session.user.index();
}

JSObject msg_creation::detail::user_make_msg(Session& session, const MsgID& id)
{
    Database::Cluster user = maindata->storage.list("User")[id.arg0];

    if (user != session.user)
    {
        h2rfp::JSObject out;
        if (user.is_null())
            return out;
        bool banned = user["banned"].get<bool>();
        out.put("banned",banned);
        out.put("id",user.index());
        if (!banned)
            out.put("name",user["name"].get<std::string>());
        else  
            out.put("name","xxx");
        out.put("points",user["points"].get<int>());
        
        return out;
    }
    else
    {
        h2rfp::JSObject gtipps;
        h2rfp::JSObject etipps;
        h2rfp::JSObject groups;
        h2rfp::JSObject item;
        for (auto rank: user["ranks"])
        {
            for (auto gtipp: rank["gametipps"]) {
                item.put_value(gtipp->index());
                gtipps.push_back(std::make_pair("", item));
            }
            item.put_value(rank["eventtipp"]->index());
            etipps.push_back(std::make_pair("", item));
        }
        for (auto group: user["groups"]) {
            item.put_value(group->index());
            groups.push_back(std::make_pair("", item));
        }

        h2rfp::JSObject answer;
        
        answer.put("id", user.index());
        answer.put("name", user["name"].get<std::string>());
        answer.add_child("eventTipps",etipps);
        answer.add_child("gameTipps",gtipps);
        answer.add_child("groups",gtipps);
        answer.put("points", user["points"].get<int>());
        answer.put("permission.eventAnnounce", user["perm_eventAnnounce"].get<bool>());
        answer.put("permission.eventReport",   user["perm_eventReport"].get<bool>());
        answer.put("permission.gameAnnounce",  user["perm_gameAnnounce"].get<bool>());
        answer.put("permission.gameReport",    user["perm_gameReport"].get<bool>());
        answer.put("permission.groupCreate",   user["perm_groupCreate"].get<bool>());
        answer.put("permission.console",       user["perm_console"].get<bool>());
        answer.put("banned", user["banned"].get<bool>());
        return answer;
    }
}