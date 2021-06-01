#include "location.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::detail::location_list_individual(Session&, const MsgID&)
{
    return false;
}

JSObject msg_creation::detail::location_list(Session& session, const MsgID& msgid)
{
    JSObject out;
    JSObject item;
    for (Database::Cluster loc: maindata->storage.list("Location"))
    {
        item.put_value(loc["name"].get<std::string>());
        out.push_back({"",item});
    }
    return out;
}