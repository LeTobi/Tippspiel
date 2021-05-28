#include "account.h"
#include "../misc/time.h"
#include "../misc/response_util.h"
#include "../main-data.h"
#include "../sessionTasks/all.h"
#include "../sessionTasks/token.h"
#include "../dataEdit/user.h"

using namespace tobilib;
using namespace h2rfp;

void msg_handler::make_new_user(Session& session, const Message& msg)
{
    if (!check_parameter(session,msg,"name") ||
        !check_parameter(session,msg,"email"))
        return;

    StringPlus name = msg.data.get("name","");
    StringPlus email = msg.data.get("email","");
    StringPlus lang = msg.data.get("lang","");

    for (Database::Cluster user: maindata->storage.list("User"))
    {
        if (StringPlus(user["name"].get<std::string>()).toLowerCase() == name.toLowerCase())
        {
            h2rfp::JSObject answer = make_user_error(ERROR_USERNAME_TAKEN);
            answer.put("data.name",name);
            return_result(session,msg,answer);
            return;
        }
        if (StringPlus(user["email"].get<std::string>()).toLowerCase() == email.toLowerCase())
        {
            h2rfp::JSObject answer = make_user_error(ERROR_EMAIL_TAKEN);
            answer.put("data.email",email);
            return_result(session,msg,answer);
            return;
        }
    }

    if (name.size()<4)
    {
        h2rfp::JSObject answer = make_user_error(ERROR_USERNAME_TAKEN);
        answer.put("data.name",name);
        return_result(session,msg,answer);
        return;
    }

    session.tasks.registration.make_new_user(name,email,lang,msg.id);
}

void msg_handler::restore_token(Session& session, const Message& msg)
{
    Database::Cluster target_user;
    StringPlus email = msg.data.get("email","");
    StringPlus lang = msg.data.get("lang","");
    if (!email.empty())
    {
        for (Database::Cluster user: maindata->storage.list("User"))
        {
            if (StringPlus(user["email"].get<std::string>()).toLowerCase() == email.toLowerCase())
            {
                target_user = user;
                break;
            }
        }
        if (target_user.is_null())
        {
            h2rfp::JSObject answer = make_user_error(ERROR_EMAIL_NOT_FOUND);
            answer.put("email",email);
            return_result(session,msg,answer);
            return;
        }
    }
    else if (!session.user.is_null())
    {
        target_user = session.user;
    }
    else
    {
        return_client_error(session,msg,"Parameter fehlt: email");
        return;
    }

    Time lastrecovery = target_user["lastrecovery"].get<int>();
    int cooldown = lastrecovery + 15*60 - get_time();
    if (cooldown > 0)
    {
        h2rfp::JSObject answer = make_user_error(ERROR_COOLDOWN);
        answer.put("data.time",interval_to_string(cooldown));
        return_result(session,msg,answer);
        return;
    }
    data_edit::set_user_lastrecovery(target_user, get_time());

    session.tasks.token_restore.restore_token(target_user, lang, msg.id);
}