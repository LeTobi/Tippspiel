#include "event.h"
#include "../misc/response_util.h"
#include "../main-data.h"
#include "../dataEdit/event.h"
#include "../tasks/all.h"
#include "../tasks/push.h"
#include "../misc/time.h"

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

    JSObject answer = make_result();
    answer.put("data.id",tipp.index());
    return_result(session,msg,answer);
}

void msg_handler::event_report(Session& session, Message& msg)
{
    if (!check_login(session,msg) ||
        !check_permission(session,msg,"perm_eventReport") ||
        !check_parameter(session,msg,"event") ||
        !check_parameter(session,msg,"winner") ||
        !check_parameter(session,msg,"topscorer"))
        return;

    Database::Cluster event = maindata->storage.list("Event")[msg.data.get("event",0)];
    Database::Cluster winner = maindata->storage.list("Team")[msg.data.get("winner",0)];
    Database::Cluster topscorer = maindata->storage.list("Player")[msg.data.get("topscorer",0)];

    if (event.is_null())
    {
        return_client_error(session,msg,"Der Anlass existiert nicht");
        return;
    }
    if (winner.is_null())
    {
        return_client_error(session,msg,"Das Siegerteam existiert nicht");
        return;
    }
    if (topscorer.is_null())
    {
        return_client_error(session,msg,"Der Topscorer existiert nicht");
        return;
    }

    if (event["deadline"].get<int>() > get_time())
    {
        return_client_error(session,msg,"Dieses Event ist noch offen fuer tipps und kann nicht beendet werden");
        return;
    }

    data_edit::report_event(event,session.user,winner,topscorer);
    maindata->tasks.push.event_results(event);

    return_result(session,msg,make_result());
}