#include "signin.h"
#include "../main-data.h"
#include "../updateTracker.h"
#include "../misc/response_util.h"
#include "../misc/enums.h"
#include "../misc/time.h"
#include "../dataEdit/token.h"
#include "../dataEdit/user.h"
#include "../sessionTasks/all.h"
#include "../sessionTasks/signout.h"
#include "../sessionTasks/cooldown.h"

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

    int cooldown = session.tasks.cooldown.login_cooldown_get();
    if (cooldown>0)
    {
        h2rfp::JSObject answer = make_user_error(ERROR_COOLDOWN);
        answer.put("data.time",interval_to_string(cooldown));
        return_result(session,msg,answer);
        return;
    }

    session.user = data_edit::check_token(token);

    if (session.user.is_null())
    {
        return_result(session,msg,make_user_error(ERROR_LOGIN_DENIED));
        session.tasks.cooldown.login_fail();
        return;
    }

    session.tasks.cooldown.login_success();

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

void msg_handler::signout(Session& session, h2rfp::Message& msg)
{
    if (!check_login(session,msg))
        return;
    
    session.tasks.signout.signout(msg.id);
}