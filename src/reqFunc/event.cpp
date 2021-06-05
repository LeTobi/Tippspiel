#include "event.h"
#include "../misc/response_util.h"
#include "../main-data.h"
#include "../dataEdit/event.h"

using namespace tobilib;
using namespace h2rfp;

void msg_handler::event_tipp(Session& session, Message& msg)
{
    if (!check_login(session,msg) ||
        !check_parameter(session,msg,"event") ||
        !check_parameter(session,msg,"winner") ||
        !check_parameter(session,msg,"topscorer"))
        return;

    int eventid = msg.data.get("event",0);
    int winnerid = msg.data.get("winner",0);
    int topscorerid = msg.data.get("topscorer",0);
    Database::Cluster event = maindata->storage.list("Event")[eventid];
    Database::Cluster winner = maindata->storage.list("Team")[winnerid];
    Database::Cluster topscorer = maindata->storage.list("Player")[topscorerid];

    if (event.is_null()) {
        return_client_error(session,msg,"Event nicht gefunden");
        return;
    }
    // topscorer und winner können undefiniert sein.

    Database::Cluster tipp = data_edit::get_event_tipp(session.user,event,true);
    data_edit::set_event_tipp(tipp,winner,topscorer);

    return_result(session,msg,make_result());
}