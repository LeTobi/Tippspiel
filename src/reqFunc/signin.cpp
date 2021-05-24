#include "signin.h"
#include "../main-data.h"
#include "../updateTracker.h"
#include "../misc/response_util.h"
#include "../misc/enums.h"
#include "../misc/time.h"
#include "../dataEdit/token.h"
#include "../dataEdit/user.h"

using namespace tobilib;

void msg_handler::signin(Session& session, h2rfp::Message& msg)
{
    bool is_retry = msg.data.get("retry",false);
    std::string token = msg.data.get("token","");

    if (!session.user.is_null())
    {
        return_result(session,msg,make_user_error(ERROR_ALREADY_LOGGED_IN));
        return;
    }

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

    if (session.user["banned"].get<bool>())
    {
        session.user = Database::Cluster();
        h2rfp::JSObject answer = make_user_error(ERROR_BANNED);
        return_result(session,msg,answer);
        return;
    }

    data_edit::set_user_last_login(session.user);
    session.log << session.user["name"].get<std::string>() << " ist nun eingeloggt" << std::endl;

    h2rfp::JSObject answer = make_result();
    int lastknown = session.user["lastupdate"].get<int>();
    int lastsupport = maindata->updateTracker.horizon();
    if (is_retry && lastknown > lastsupport) {
        answer.put("data.upToDate",true);
        answer.put_child("data.updates",maindata->updateTracker.get_history(lastknown));
    } else {
        answer.put("data.upToDate",false);
    }
    data_edit::set_user_sync(session.user,get_time());
    
    return_result(session,msg,answer);
}