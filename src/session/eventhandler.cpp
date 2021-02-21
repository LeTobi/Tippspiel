#include "eventhandler.h"
#include "../manipulation/consoleinput.h"
#include "../manipulation/game.h"
#include "utils.h"
#include "sync.h"
#include <sstream>

void signup(Client& client, h2rfp::Message msg)
{
    TokenEmail::Input input;
    input.name = msg.data.get("name","");
    // input.token nach prüfung
    input.email = StringPlus(msg.data.get("email","")).toLowerCase();
    input.jsid = msg.id;

    if (input.name.empty()) {
        return_client_error(client,msg,"name parameter fehlt");
        return;
    }
    if (input.email.empty()) {
        return_client_error(client,msg,"email parameter fehlt");
        return;
    }

    for (Database::Cluster user: data->database.list("User"))
    {
        if (StringPlus(user["name"].get<std::string>()).toLowerCase() == StringPlus(input.name).toLowerCase())
        {
            h2rfp::JSObject answer = make_user_error(ERROR_USERNAME_TAKEN);
            answer.put("data.name",input.name);
            return_result(client,msg,answer);
            return;
        }
        if (user["email"].get<std::string>() == input.email)
        {
            h2rfp::JSObject answer = make_user_error(ERROR_EMAIL_TAKEN);
            answer.put("data.email",input.email);
            return_result(client,msg,answer);
            return;
        }
    }

    bool success = false;
    FlagRequest lock = data->database.begin_critical_operation();
        Database::Cluster user = data->database.list("User").emplace();
        input.token = make_new_token(user);
        if (!input.token.empty()) {
            success = true;
            user["name"].set(input.name);
            user["email"].set(input.email);
            user["token"].set(input.token);
            user["registerdate"].set(get_time());
            user["lastlogin"].set( user["registerdate"].get<int>() );
            user["points"].set(0);
            user["perm_eventAnnounce"].set(false);
            user["perm_eventReport"].set(false);
            user["perm_gameAnnounce"].set(true);
            user["perm_gameReport"].set(true);
        } else {
            user.erase();
        }
    data->database.end_critical_operation(lock);

    if (!success)
    {
        return_server_error(client,msg,"Das Token konnte nicht generiert werden. (Fehler in der Datei?)");
        return;
    }
    
    if (!data->database.is_good())
    {
        return_server_error(client,msg,"Kritischer Datenbankfehler");
        return;
    }

    client.emailtask = data->emails.tasks.makeTask(input);
}

void signup_complete(Client& client, TokenEmail::Result res)
{
    h2rfp::JSObject answer;
    answer.put("state",res.result);
    answer.put("error",ERROR_NONE);
    client.endpoint.respond(res.jsid, answer);
}

void signin(Client& client, h2rfp::Message msg)
{
    bool is_retry = msg.data.get("retry",false);
    std::string token = msg.data.get("token","");

    if (token.empty())
    {
        return_client_error(client,msg,"Es wurde kein token angegeben");
        return;
    }

    client.user = check_token(token);

    if (client.user.is_null())
    {
        return_result(client,msg,make_user_error(ERROR_LOGIN_DENIED));
        return;
    }

    client.user["lastupdate"].set( get_time() );

    h2rfp::JSObject answer = make_result();
    int lastknown = client.user["lastupdate"].get<int>();
    int lastsupport = data->virtualdb.get_history_support();
    if (is_retry && lastknown > lastsupport) {
        answer.put("data.upToDate",true);
        answer.put_child("data.updates",data->virtualdb.get_history(lastknown));
    } else {
        answer.put("data.upToDate",false);
    }
    
    return_result(client,msg,answer);
}

void inform_identity(Client& client, h2rfp::Message msg)
{
    if (!check_login(client,msg))
        return;

    h2rfp::JSObject gtipps;
    h2rfp::JSObject etipps;
    h2rfp::JSObject groups;
    h2rfp::JSObject item;
    for (auto gtipp: client.user["gameTipps"]) {
        item.put_value(gtipp->index());
        gtipps.push_back(std::make_pair("", item));
    }
    for (auto etipp: client.user["eventTipps"]) {
        item.put_value(etipp->index());
        etipps.push_back(std::make_pair("", item));
    }
    for (auto group: client.user["groups"]) {
        item.put_value(group->index());
        groups.push_back(std::make_pair("", item));
    }

    h2rfp::JSObject answer;
    
    answer.put("state",RESULT_SUCCESS);
    answer.put("error",ERROR_NONE);
    answer.put("data.id", client.user.index());
    answer.put("data.name", client.user["name"].get<std::string>());
    answer.add_child("data.eventTipps",etipps);
    answer.add_child("data.gameTipps",gtipps);
    answer.add_child("data.groups",gtipps);
    answer.put("data.points", client.user["points"].get<int>());
    answer.put("data.permission.eventAnnounce", client.user["perm_eventAnnounce"].get<bool>());
    answer.put("data.permission.eventReport", client.user["perm_eventReport"].get<bool>());
    answer.put("data.permission.gameAnnounce", client.user["perm_gameAnnounce"].get<bool>());
    answer.put("data.permission.gameReport", client.user["perm_gameReport"].get<bool>());
    answer.put("data.permission.console", client.user["perm_console"].get<bool>());
    client.endpoint.respond(msg.id, answer);
}

void console_input(Client& client, h2rfp::Message msg)
{
    if (!check_login(client,msg) ||
        !check_permission(client,msg,"perm_console") )
        return;

    std::string line = msg.data.get("cmd","");
    size_t space = line.find(' ');
    if (space==std::string::npos)
        space = line.size();
    std::string cmd = line.substr(0,space);
    std::string rest = line.substr(space);
    
    if (cmd == "access") {
        cmd_access(client,msg,rest);
    }
    else if (cmd == "print") {
        cmd_print(client,msg,rest);
    }
    else if (cmd == "cache") {
        cmd_cache(client,msg,rest);
    }
    else if (cmd=="guard") {
        cmd_guard(client,msg,rest);
    }
    else {
        h2rfp::JSObject answer = make_result();
        answer.put("data.text",std::string("unbekannter befehl: ")+cmd);
        return_result(client,msg,answer);
    }

}

void serve_data(Client& client, h2rfp::Message msg)
{
    std::string table = msg.data.get("table","");
    int tid = VirtualDB::get_table(table);
    if (tid < 0)
    {
        return_client_error(client,msg,std::string("Tabelle nicht bekannt: ")+table);
        return;
    }

    h2rfp::JSObject ids;
    try {
        ids = msg.data.get_child("ids");
    } catch (boost::property_tree::ptree_error& e) {
        return_client_error(client,msg,"Keine Ids angegeben");
        return;
    }

    h2rfp::JSObject outlist;
    for (auto& item: ids)
    {
        unsigned int id = item.second.get_value(0);
        if (id==0)
        {
            return_client_error(client,msg,"Eine Id stimmt nicht");
            return;
        }
        VirtualDB::Entry entry (tid, id);
        outlist.push_back(std::make_pair("", custom_client_data(client,entry)));
    }

    h2rfp::JSObject output = make_result();
    output.put_child("data", outlist);
    return_result(client,msg,output);
}