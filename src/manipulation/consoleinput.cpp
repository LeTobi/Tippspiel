#include <tobilib/database/manipulation/commands.h>
#include <tobilib/database/manipulation/export.h>
#include <sstream>
#include "consoleinput.h"
#include "../components/config.h"

void cmd_echo(Client& client, h2rfp::Message msg, const std::string& txt)
{
    h2rfp::JSObject answer;
    answer.put("state",RESULT_SUCCESS);
    answer.put("error",ERROR_NONE);
    answer.put("data.text",txt);
    client.endpoint.respond(msg.id, answer);
}

void cmd_access(Client& client, h2rfp::Message msg, const std::string& cmd)
{
    cmd_echo(client, msg, database_tools::command(data->database, cmd) );
}

void cmd_print(Client& client, h2rfp::Message msg, const std::string& cmd)
{
    std::string path = get_config("../../config/export.txt");
    if (path.empty())
    {
        cmd_echo(client, msg, "export.txt kann nicht gelesen werden.");
        return;
    }

    std::string table;
    std::stringstream input(cmd);
    input >> table;
    if (!input)
    {
        cmd_echo(client,msg, "print: Es muss eine Tabelle angegeben werden");
    }

    database_tools::Result result;

    if (table == "all")
    {
        result = database_tools::export_database(data->database,path);
        if (!result)
        {
            cmd_echo(client,msg,result.info);
            return;
        }
        cmd_echo(client,msg,"Datenbank wurde vollständig exportiert");
        return;
    }

    result = database_tools::export_table(data->database, table, path);
    if (!result)
    {
        cmd_echo(client,msg,result.info);
        return;
    }
    cmd_echo(client,msg,"Die Tabelle wurde exportiert");
}

void cmd_cache(Client& client, h2rfp::Message msg, const std::string& in)
{
    std::stringstream input(in);
    std::stringstream output;
    std::string cmd;
    input >> cmd;
    if (cmd=="stats")
    {
        if (data->virtualdb.stat_requests==0)
        {
            cmd_echo(client,msg,"Bisher keine Anfragen");
            return;
        }
        int percent = 100 * data->virtualdb.stat_caches / data->virtualdb.stat_requests;
        output << data->virtualdb.stat_caches << "/" << data->virtualdb.stat_requests << " (" << percent << "%) requests aus Cache beantwortet";
        cmd_echo(client,msg,output.str());
        return;
    }
    if (cmd=="newstats")
    {
        data->virtualdb.stat_caches = 0;
        data->virtualdb.stat_requests = 0;
        cmd_echo(client,msg,"Statistik wurde zurückgesetzt.");
        return;
    }
    else if (cmd=="find")
    {
        std::string table;
        unsigned int index;
        input >> table >> index;
        if (!input)
        {
            cmd_echo(client,msg,"find: erwarte tabelle und index");
            return;
        }
        int tid = VirtualDB::get_table(table);
        if (tid<0)
        {
            cmd_echo(client,msg,"tabelle existiert nicht");
            return;
        }
        int result = data->virtualdb.find_entry(VirtualDB::Entry(tid,index));
        if (result<0)
        {
            cmd_echo(client,msg,"objekt ist nicht im cache");
            return;
        }
        output << "objekt ist an der Stelle " << result;
        cmd_echo(client,msg,output.str());
        return;
    }
    else if (cmd=="update")
    {
        std::string table;
        unsigned int id;
        Time urgency;
        input >> table >> id;
        if (!input)
        {
            cmd_echo(client,msg,"update: erwarte tabelle und index");
            return;
        }
        input >> urgency;
        if (!input)
            urgency = VirtualDB::WAIT_NOT;
        Database::ClusterType type = data->database.getType(table);
        if (type == Database::ClusterType::invalid)
        {
            cmd_echo(client,msg,"tabelle existiert nicht");
            return;
        }
        Database::Cluster cluster = data->database.list(table)[id];
        if (cluster.is_null())
        {
            cmd_echo(client,msg,"ungültiger index");
            return;
        }
        data->virtualdb.update(cluster,urgency);
        cmd_echo(client,msg,std::string("Update wurde eingereicht mit Verzögerung ")+std::to_string(urgency));
        return;
    }
    else if (cmd=="clear")
    {
        data->virtualdb.clear_cache();
        cmd_echo(client,msg,"Der Cache wurde geleert");
        return;
    }
    else {
        cmd_echo(client,msg,"Unbekannter Befehl");
    }
}

void cmd_guard(Client& client, h2rfp::Message msg, const std::string& in)
{
    std::stringstream input (in);
    std::string cmd;
    input >> cmd;
    if (!input) {
        cmd_echo(client,msg,"gard: erwarte anweisung");
        return;
    }
    if (cmd=="watch")
    {
        std::map<VirtualDB::Entry,int> active_users;
        int sum = 0;
        for (Client& cli: data->clients) {
            if (cli.endpoint.status() == h2rfp::EndpointStatus::connected)
            {
                sum++;
                VirtualDB::Entry entry = cli.user;
                if (active_users.count(entry)==0)
                    active_users[entry] = 1;
                else
                    active_users[entry]++;
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
        cmd_echo(client,msg,answer.str());
        return;
    }

}