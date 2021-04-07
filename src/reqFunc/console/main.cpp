#include "main.h"
#include "../../misc/config.h"
#include "../../misc/response_util.h"
#include "../../main-data.h"
#include "../../msgCache.h"
#include <tobilib/database/manipulation/commands.h>
#include <tobilib/database/manipulation/export.h>

#include <map>

using namespace tobilib;
using namespace h2rfp;

const std::map<std::string,MsgType> msgtypes {
    {"Event",MsgType::event},
    {"EventTipp",MsgType::eventTipp},
    {"Game",MsgType::game},
    {"GameTipp",MsgType::gameTipp},
    {"Group",MsgType::group},
    {"HotGames",MsgType::hotGames},
    {"Locations",MsgType::locations},
    {"Player",MsgType::player},
    {"Team",MsgType::team},
    {"User",MsgType::user}
};

void cmd_echo(Session& session, const Message& msg, const std::string& txt)
{
    h2rfp::JSObject answer = make_result();
    answer.put("data.text",txt);
    return_result(session,msg,answer);
}

void cmd_access(Session& session, const Message& msg, const std::string& cmd)
{
    cmd_echo(session, msg, database_tools::command(maindata->storage, cmd) );
}

void cmd_print(Session& session, const Message& msg, const std::string& cmd)
{
    std::string path = get_config("../../config/export.txt");
    if (path.empty())
    {
        cmd_echo(session, msg, "export.txt kann nicht gelesen werden.");
        return;
    }

    std::string table;
    std::stringstream input(cmd);
    input >> table;
    if (!input)
    {
        cmd_echo(session,msg, "print: Es muss eine Tabelle angegeben werden");
    }

    database_tools::Result result;

    if (table == "all")
    {
        result = database_tools::export_database(maindata->storage,path);
        if (!result)
        {
            cmd_echo(session,msg,result.info);
            return;
        }
        cmd_echo(session,msg,"Datenbank wurde vollständig exportiert");
        return;
    }

    result = database_tools::export_table(maindata->storage, table, path);
    if (!result)
    {
        cmd_echo(session,msg,result.info);
        return;
    }
    cmd_echo(session,msg,"Die Tabelle wurde exportiert");
}

void cmd_cache(Session& session, const Message& msg, const std::string& in)
{
    std::stringstream input(in);
    std::stringstream output;
    std::string cmd;
    input >> cmd;
    if (cmd=="stats")
    {
        if (maindata->cache.stat_requests==0)
        {
            cmd_echo(session,msg,"Bisher keine Anfragen");
            return;
        }
        int percent = 100 * maindata->cache.stat_hits / maindata->cache.stat_requests;
        output << maindata->cache.stat_hits << "/" << maindata->cache.stat_requests << " (" << percent << "%) requests aus Cache beantwortet";
        cmd_echo(session,msg,output.str());
        return;
    }
    if (cmd=="newstats")
    {
        maindata->cache.stat_hits = 0;
        maindata->cache.stat_requests = 0;
        cmd_echo(session,msg,"Statistik wurde zurückgesetzt.");
        return;
    }
    else if (cmd=="msgtypes")
    {
        std::string output;
        for (auto& item: msgtypes)
            output += item.first + "\n";
        cmd_echo(session,msg,output);
        return;
    }
    else if (cmd=="find")
    {
        std::string typestr;
        unsigned int arg0;
        input >> typestr;
        if (!input)
        {
            cmd_echo(session,msg,"find: erwarte msgtype");
            return;
        }
        input >> arg0;
        if (!input)
            arg0 = 0;
        if (msgtypes.count(typestr)==0)
        {
            cmd_echo(session,msg,"Nachrichtentyp existiert nicht");
            return;
        }
        MsgID target_msg (msgtypes.at(typestr),arg0);
        int result = maindata->cache.find_entry(target_msg);
        if (result<0)
        {
            cmd_echo(session,msg,"die Nachricht ist nicht im cache");
            return;
        }
        output << "Nachricht ist an der Stelle " << result;
        cmd_echo(session,msg,output.str());
        return;
    }
    else if (cmd=="clear")
    {
        maindata->cache.clear();
        cmd_echo(session,msg,"Der Cache wurde geleert");
        return;
    }
    else {
        cmd_echo(session,msg,"Unbekannter Befehl");
    }
}

void cmd_guard(Session& session, const Message& msg, const std::string& in)
{
    std::stringstream input (in);
    std::string cmd;
    input >> cmd;
    if (!input) {
        cmd_echo(session,msg,"gard: erwarte anweisung");
        return;
    }
    if (cmd=="watch")
    {
        std::map<Database::Cluster,int> active_users;
        int sum = 0;
        for (Session& sess: maindata->sessions) {
            if (sess.client.status() == h2rfp::EndpointStatus::connected)
            {
                sum++;
                if (active_users.count(sess.user)==0)
                    active_users[sess.user] = 1;
                else
                    active_users[sess.user]++;
            }
        }
        std::stringstream answer;
        answer << active_users.size() << " users mit " << sum << " clients verbunden";
        for (auto& item: active_users)
        {
            Database::Cluster cli_user = item.first;
            if (cli_user.is_null())
                answer << "\n" << item.second << "x not logged in";
            else
                answer << "\nid " << cli_user.index() << " (" << cli_user["name"].get<std::string>() << ") connected " << item.second << "x";
        }
        cmd_echo(session,msg,answer.str());
        return;
    }

}