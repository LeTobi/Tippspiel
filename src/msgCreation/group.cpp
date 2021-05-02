#include "group.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::group_is_individual(Session& session, const MsgID& id)
{
    Database::Cluster group = maindata->storage.list("Group")[id.arg0];
    return *group["admin"] == session.user;
}

JSObject msg_creation::detail::group_make_msg(Session& session, const MsgID& id)
{
    Database::Cluster group = maindata->storage.list("Group")[id.arg0];
    h2rfp::JSObject out;
    if (group.is_null())
        return out;
    out.put("id",group.index());
    out.put("name",group["name"].get<std::string>());
    out.put("admin",group["admin"]->index());
    h2rfp::JSObject members;
    for (auto member: group["users"]) {
        h2rfp::JSObject item;
        item.put_value(member->index());
        members.push_back(std::make_pair("",item));
    }
    out.put_child("users",members);

    if (session.user != *group["admin"])
        return out;

    out.put("token",group["token"].get<std::string>());
    return out;
}