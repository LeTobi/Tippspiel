#include "signin.h"
#include "../main-data.h"
#include "../misc/response_util.h"
#include "../misc/enums.h"
#include "../misc/time.h"
#include "../dataEdit/token.h"

using namespace tobilib;

void msg_handler::signin(Session& session, h2rfp::Message& msg)
{
    bool is_retry = msg.data.get("retry",false);
    std::string token = msg.data.get("token","");

    if (token.empty())
    {
        return_client_error(session,msg,"Es wurde kein token angegeben");
        return;
    }

    session.user = data_edit::check_token(token);

    if (session.user.is_null())
    {
        return_result(session,msg,make_user_error(ERROR_LOGIN_DENIED));
        return;
    }

    session.log << session.user["name"].get<std::string>() << " ist nun eingeloggt" << std::endl;
    session.user["lastupdate"].set( get_time() );

    h2rfp::JSObject answer = make_result();
    int lastknown = session.user["lastupdate"].get<int>();
    int lastsupport = maindata->updateTracker.horizon();
    if (is_retry && lastknown > lastsupport) {
        answer.put("data.upToDate",true);
        answer.put_child("data.updates",maindata->updateTracker.get_history(lastknown));
    } else {
        answer.put("data.upToDate",false);
    }
    answer.put("data.upToDate",false);
    
    return_result(session,msg,answer);
}