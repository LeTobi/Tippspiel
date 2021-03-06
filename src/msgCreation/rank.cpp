#include "rank.h"
#include "../main-data.h"

using namespace tobilib;

bool msg_creation::detail::event_rank_individual(Session& session, MsgID id)
{
    return false;
}

struct OnlyThisFile_RankOrder {
    bool operator()(const Database::Cluster& a, const Database::Cluster& b) {
        return a["points"].get<int>() > b["points"].get<int>();
    }
};

h2rfp::JSObject msg_creation::detail::event_rank_msg(Session& session, MsgID id)
{
    std::multiset<Database::Cluster,OnlyThisFile_RankOrder> ranking;
    Database::Cluster event = maindata->storage.list("Event")[id.arg0];
    for (Database::Member rptr: event["ranks"])
        ranking.insert(*rptr);

    h2rfp::JSObject output;
    h2rfp::JSObject item;
    OnlyThisFile_RankOrder is_better;
    unsigned int global_rank = 1;
    Database::Cluster last_rank;
    for (Database::Cluster rank: ranking)
    {
        if (is_better(last_rank,rank))
            global_rank++;
        item.put("user",rank["user"]->index());
        item.put("points",rank["points"].get<int>());
        item.put("rank",global_rank);
        output.push_back({"",item});
        last_rank = rank;
    }
    return output;
}