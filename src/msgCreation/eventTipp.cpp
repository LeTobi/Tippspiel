#include "eventTipp.h"
#include "../main-data.h"
#include "../misc/time.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::eventtipp_is_individual(Session& session, const MsgID& id)
{
    Database::Cluster etipp = maindata->storage.list("EventTipp")[id.arg0];
    if (etipp["event"]["deadline"].get<int>() > get_time()
        && *etipp["user"] == session.user)
        return true;
    return false;
}

JSObject msg_creation::detail::eventtipp_make_msg(Session& session, const MsgID& id)
{
    Database::Cluster etipp = maindata->storage.list("EventTipp")[id.arg0];
    h2rfp::JSObject out;
    if (etipp.is_null())
        return out;
    out.put("id",etipp.index());
    out.put("event",etipp["event"]->index());
    out.put("user",etipp["user"]->index());
    out.put("reward",etipp["reward"].get<int>());

    if (etipp["event"]["deadline"].get<int>() > get_time()
        && *etipp["user"] != session.user)
        return out;

    out.put("winner",etipp["winner"]->index());
    out.put("topscorer",etipp["topscorer"]->index());
    return out;
}