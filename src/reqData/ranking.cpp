#include "ranking.h"
#include "../misc/response_util.h"
#include "../msgCache.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

void msg_handler::get_event_ranking(Session& session, const Message& msg)
{
    if (!check_login(session,msg)
        || !check_parameter(session,msg,"event"))
        return;

    Database::Cluster event = maindata->storage.list("Event")[msg.data.get("event",0)];
    if (event.is_null()) {
        return_client_error(session,msg,"Das Event existiert nicht");
        return;
    }

    MsgID id (MsgType::eventRank,event.index());

    JSObject answer = make_result();
    answer.put_child("data",maindata->cache.get_data(session,id));
    return_result(session,msg,answer);
}