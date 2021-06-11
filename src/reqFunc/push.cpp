#include "push.h"
#include "../misc/response_util.h"
#include "../main-data.h"
#include "../dataEdit/push.h"

void msg_handler::push_enable(Session& session, const tobilib::h2rfp::Message& msg)
{
    if (!check_login(session,msg))
        return;
    
    std::string endpoint = msg.data.get("subscription.endpoint","");
    std::string p256dh = msg.data.get("subscription.keys.p256dh","");
    std::string auth = msg.data.get("subscription.keys.auth","");

    if (endpoint.empty()) {
        return_client_error(session,msg,"subscription.endpoint fehlt");
        return;
    }
    if (p256dh.empty()) {
        return_client_error(session,msg,"subscription.keys.p256dh fehlt");
        return;
    }
    if (auth.empty()) {
        return_client_error(session,msg,"subscription.keys.auth fehlt");
        return;
    }

    if (endpoint.size() > 250) {
        return_server_error(session,msg,"Der Push endpoint ist zu lang.");
        maindata->log << "FEHLER: Es wurde nicht genug platz fuer einen Endpoint reserviert. Anfrage von size " << endpoint.size() << std::endl;
        return;
    }
    if (p256dh.size() > 100) {
        return_server_error(session,msg,"Der Push public key ist zu lang.");
        maindata->log << "FEHLER: Es wurde nicht genug platz fuer einen push public key reserviert. Anfrage von size " << p256dh.size() << std::endl;
        return;
    }
    if (auth.size() > 50) {
        return_server_error(session,msg,"Der Push auth key ist zu lang.");
        maindata->log << "FEHLER: Es wurde nicht genug platz fuer einen push auth key reserviert. Anfrage von size " << auth.size() << std::endl;
        return;
    }

    data_edit::add_push(session.user,endpoint,p256dh,auth);
    return_result(session,msg,make_result());
    return;
}

void msg_handler::push_disable(Session& session, const tobilib::h2rfp::Message& msg)
{
    if (!check_login(session,msg)
        || !check_parameter(session,msg,"endpoint"))
        return;
    
    std::string endpoint = msg.data.get("endpoint","");

    data_edit::remove_push(endpoint);

    return_result(session,msg,make_result());
    return;
}