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
    std::fstream fs ("../config/tokens.txt");
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

bool check_login(Client& client, unsigned int msgid)
{
    if (!client.user.is_null())
        return true;
    if (msgid == 0)
        return false;

    h2rfp::JSObject answer;
    answer.put("state",RFPResult::client_err);
    answer.put("error",ErrCode::NO_ERROR);
    answer.put("data.info","Login fehlt");
    client.endpoint.respond(msgid, answer);
    return false;
}

bool check_permission(const std::string& permission, Client& client, unsigned int msgid)
{
    if (client.user[permission].get<bool>())
        return true;
    if (msgid==0)
        return false;
    
    h2rfp::JSObject answer;
    answer.put("state",RFPResult::client_err);
    answer.put("error",ErrCode::NO_ERROR);
    answer.put("data.info",std::string("Die Berechtigung ") + permission + " fehlt");
    client.endpoint.respond(msgid, answer);
    return false;
}