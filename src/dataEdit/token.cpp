#include "token.h"
#include "../main-data.h"

using namespace tobilib;

Database::Cluster data_edit::check_token(std::string token)
{
    size_t pos = token.find('-');
    if (pos==std::string::npos)
        return Database::Cluster();
    StringPlus intstr = token.substr(0,pos);
    if (!intstr.isInt())
        return Database::Cluster();
    Database::Cluster user = maindata->storage.list("User")[intstr.toInt()];
    if (user["token"].get<std::string>() == token)
        return user;
    else
        return Database::Cluster();
}

std::string data_edit::make_new_token(Database::Cluster user)
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
