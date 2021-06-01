#include "gameTipp.h"
#include "../main-data.h"
#include "../misc/time.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::gametipp_is_individual(Session& session, const MsgID& id)
{
    Database::Cluster gtipp = maindata->storage.list("GameTipp")[id.arg0];
    if (gtipp["game"]["start"].get<int>() > get_time()
        && *gtipp["user"] == session.user)
        return true;
    return false;
}

JSObject msg_creation::detail::gametipp_make_msg(Session& session, const MsgID& id)
{
    Database::Cluster gtipp = maindata->storage.list("GameTipp")[id.arg0];
    h2rfp::JSObject out;
    if (gtipp.is_null())
        return out;
    out.put("id",gtipp.index());
    out.put("game",gtipp["game"]->index());
    out.put("user",gtipp["user"]->index());
    out.put("reward",gtipp["reward"].get<int>());
    out.put("tippkat",gtipp["tippkat"].get<int>());
    out.put("goals",gtipp["goals"].get<int>());
    out.put("bonus",gtipp["penaltyBonus"].get<bool>());

    if (gtipp["game"]["start"].get<int>() > get_time()
        && *gtipp["user"] != session.user)
        return out;

    out.put("bet1",gtipp["bet"][0].get<int>());
    out.put("bet2",gtipp["bet"][1].get<int>());
    out.put("winner",gtipp["winner"]->index());
    out.put("topscorer",gtipp["topscorer"]->index());
    return out;
}