#include "response_util.h"

using namespace tobilib;

bool check_login(Session& session, h2rfp::Message msg)
{
    if (!session.user.is_null())
        return true;
    if (msg.id == 0)
        return false;

    session.log << "Anfrage abgewiesen, login fehlt" << std::endl;
    h2rfp::JSObject answer;
    answer.put("state",RESULT_CLIENT_ERROR);
    answer.put("error",ERROR_NONE);
    answer.put("data.info","Login fehlt");
    session.client.respond(msg.id, answer);
    return false;
}

bool check_permission(Session& session, h2rfp::Message msg, const std::string& permission)
{
    if (session.user[permission].get<bool>())
        return true;
    if (msg.id==0)
        return false;
    
    session.log << "Anfrage abgewiesen, Berechtigung fehlt" << std::endl;
    h2rfp::JSObject answer;
    answer.put("state",RESULT_CLIENT_ERROR);
    answer.put("error",ERROR_NONE);
    answer.put("data.info",std::string("Die Berechtigung ") + permission + " fehlt");
    session.client.respond(msg.id, answer);
    return false;
}

bool check_parameter(Session& session, h2rfp::Message msg, const std::string& param)
{
    if (msg.data.find(param) != msg.data.not_found())
        return true;
    session.log << "Anfrage falsch formattiert, erwarte '" << param << "'" << std::endl;
    h2rfp::JSObject answer;
    answer.put("state",RESULT_CLIENT_ERROR);
    answer.put("error",ERROR_NONE);
    answer.put("data.info",std::string("Parameter fehlt: ")+param);
    session.client.respond(msg.id,answer);
    return false;
}

h2rfp::JSObject make_result()
{
    h2rfp::JSObject out;
    out.put("state",RESULT_SUCCESS);
    out.put("error",ERROR_NONE);
    out.put("data","");
    return out;
}

h2rfp::JSObject make_user_error(int errtype)
{
    h2rfp::JSObject out;
    out.put("state",RESULT_USER_ERROR);
    out.put("error",errtype);
    out.put("data","");
    return out;
}

void return_result(Session& session, h2rfp::Message msg, h2rfp::JSObject data)
{
    return_result(session,msg.id,data);
}

void return_result(Session& session, unsigned int msgid, h2rfp::JSObject data)
{
    session.client.respond(msgid,data);
}

void return_client_error(Session& session, h2rfp::Message msg, const std::string& info)
{
    return_client_error(session,msg.id,info);
}

void return_client_error(Session& session, unsigned int msgid, const std::string& info)
{
    if (!session.client.is_connected())
        return;
    session.log << "client request error: " << info << std::endl;
    h2rfp::JSObject out;
    out.put("state",RESULT_CLIENT_ERROR);
    out.put("error",ERROR_NONE);
    out.put("data.info",info);
    session.client.respond(msgid,out);
}

void return_server_error(Session& session, h2rfp::Message msg, const std::string& info)
{
    return_server_error(session,msg.id,info);
}

void return_server_error(Session& session, unsigned int msgid, const std::string& info)
{
    if (!session.client.is_connected())
        return;
    session.log << "server error: " << info << std::endl;
    h2rfp::JSObject out;
    out.put("state",RESULT_SERVER_ERROR);
    out.put("error",ERROR_NONE);
    out.put("data.info",info);
    session.client.respond(msgid,out);
}