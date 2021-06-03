#include "gameTipp.h"
#include "../main-data.h"
#include "../misc/time.h"
#include "../misc/enums.h"

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
    out.put("processed",gtipp["game"]["gameStatus"].get<int>()==GSTATUS_ENDED);
    out.put("reward.team",gtipp["reward_team"].get<bool>());
    out.put("reward.diff",gtipp["reward_diff"].get<bool>());
    out.put("reward.exact",gtipp["reward_exact"].get<bool>());
    out.put("reward.draw",gtipp["reward_draw"].get<bool>());
    out.put("reward.scorer",gtipp["reward_scorer"].get<int>());
    out.put("reward.sum",gtipp["reward"].get<int>());

    if (gtipp["game"]["start"].get<int>() > get_time()
        && *gtipp["user"] != session.user)
        return out;

    out.put("bet1",gtipp["bet"][0].get<int>());
    out.put("bet2",gtipp["bet"][1].get<int>());
    out.put("winner",gtipp["winner"]->index());
    out.put("topscorer",gtipp["topscorer"]->index());
    return out;
}