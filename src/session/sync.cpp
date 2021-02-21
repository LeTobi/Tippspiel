#include "sync.h"

h2rfp::JSObject custom_client_data(Client& client, VirtualDB::Entry entry)
{
    if (entry.table == TABLE_ETIPP)
    {
        Database::Cluster etipp = entry;
        if (*etipp["user"] == client.user &&
            etipp["event"]["deadline"].get<int>() > get_time())
            return private_etipp(etipp);    
    }
    if (entry.table == TABLE_GTIPP)
    {
        Database::Cluster gtipp = entry;
        if (*gtipp["user"] == client.user &&
            gtipp["game"]["start"].get<int>() > get_time())
            return private_gtipp(gtipp); 
    }
    return data->virtualdb.get_data(entry);
}

h2rfp::JSObject private_gtipp(Database::Cluster& gtipp)
{
    h2rfp::JSObject out;
    if (gtipp.is_null())
        return out;
    out.put("id",gtipp.index());
    out.put("game",gtipp["game"]->index());
    out.put("user",gtipp["user"]->index());
    out.put("bet1",gtipp["bet"][0].get<int>());
    out.put("bet2",gtipp["bet"][1].get<int>());
    out.put("winner",gtipp["winner"]->index());
    out.put("topscorer",gtipp["topscorer"]->index());
    out.put("reward",gtipp["reward"].get<int>());
    return out;
}

h2rfp::JSObject private_etipp(Database::Cluster& etipp)
{
    h2rfp::JSObject out;
    if (etipp.is_null())
        return out;
    out.put("id",etipp.index());
    out.put("event",etipp["event"]->index());
    out.put("user",etipp["user"]->index());
    out.put("winner",etipp["winner"]->index());
    out.put("topscorer",etipp["topscorer"]->index());
    out.put("reward",etipp["reward"].get<int>());
    return out;
}
