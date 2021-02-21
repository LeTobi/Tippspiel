#include "utils.h"
#include <tobilib/stringplus/stringplus.h>
#include <fstream>
#include <random>

Database::Cluster check_token(std::string token)
{
    size_t pos = token.find('-');
    if (pos==std::string::npos)
        return Database::Cluster();
    StringPlus intstr = token.substr(0,pos);
    if (!intstr.isInt())
        return Database::Cluster();
    Database::Cluster user = data->database.list("User")[intstr.toInt()];
    if (user["token"].get<std::string>() == token)
        return user;
    else
        return Database::Cluster();
}

std::string make_new_token(Database::Cluster user)
{
    std::fstream fs ("../../config/tokens.txt");
    std::string entry;
    std::vector<std::string> adjectives;
    std::vector<std::string> nouns;
    fs>>entry;
    while (fs && entry!="---") {
        adjectives.push_back(entry);
        fs>>entry;
    }
    fs>>entry;
    while (fs && entry!="---") {
        nouns.push_back(entry);
        fs>>entry;
    }
    if (!fs || nouns.size()==0 || adjectives.size()==0)
        return "";
    std::random_device rd;
    std::string token = std::to_string(user.index()) + "-";
    token+=adjectives[rd()%adjectives.size()];
    token+=nouns[rd()%nouns.size()];
    return token;
}

bool check_login(Client& client, h2rfp::Message msg)
{
    if (!client.user.is_null())
        return true;
    if (msg.id == 0)
        return false;

    h2rfp::JSObject answer;
    answer.put("state",RESULT_CLIENT_ERROR);
    answer.put("error",ERROR_NONE);
    answer.put("data.info","Login fehlt");
    client.endpoint.respond(msg.id, answer);
    return false;
}

bool check_permission(Client& client, h2rfp::Message msg, const std::string& permission)
{
    if (client.user[permission].get<bool>())
        return true;
    if (msg.id==0)
        return false;
    
    h2rfp::JSObject answer;
    answer.put("state",RESULT_CLIENT_ERROR);
    answer.put("error",ERROR_NONE);
    answer.put("data.info",std::string("Die Berechtigung ") + permission + " fehlt");
    client.endpoint.respond(msg.id, answer);
    return false;
}

bool check_parameter(Client& client, h2rfp::Message msg, const std::string& param)
{
    if (msg.data.find(param) != msg.data.not_found())
        return true;
    h2rfp::JSObject answer;
    answer.put("state",RESULT_CLIENT_ERROR);
    answer.put("error",ERROR_NONE);
    answer.put("data.info",std::string("Parameter fehlt: ")+param);
    client.endpoint.respond(msg.id,answer);
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

void return_result(Client& client, h2rfp::Message msg, h2rfp::JSObject data)
{
    client.endpoint.respond(msg.id,data);
}

void return_client_error(Client&client, h2rfp::Message msg, const std::string& info)
{
    h2rfp::JSObject out;
    out.put("state",RESULT_CLIENT_ERROR);
    out.put("error",ERROR_NONE);
    out.put("data.info",info);
    client.endpoint.respond(msg.id,out);
}

void return_server_error(Client&client, h2rfp::Message msg, const std::string& info)
{
    h2rfp::JSObject out;
    out.put("state",RESULT_SERVER_ERROR);
    out.put("error",ERROR_NONE);
    out.put("data.info",info);
    client.endpoint.respond(msg.id,out);
}