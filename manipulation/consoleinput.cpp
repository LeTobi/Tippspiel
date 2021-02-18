#include <tobilib/database/manipulation/commands.h>
#include <tobilib/database/manipulation/export.h>
#include <sstream>
#include "consoleinput.h"
#include "../components/config.h"

void cmd_echo(Client& client, h2rfp::Message msg, const std::string& txt)
{
    h2rfp::JSObject answer;
    answer.put("state",RFPResult::success);
    answer.put("error",ErrCode::NO_ERROR);
    answer.put("data.text",txt);
    client.endpoint.respond(msg.id, answer);
}

void cmd_access(Client& client, h2rfp::Message msg, const std::string& cmd)
{
    cmd_echo(client, msg, database_tools::command(data->database, cmd) );
}

void cmd_print(Client& client, h2rfp::Message msg, const std::string& cmd)
{
    std::string path = get_config("../config/export.txt");
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
        int result = data->virtualdb.find_entry(tid,index);
        if (result<0)
        {
            cmd_echo(client,msg,"objekt ist nicht im cache");
            return;
        }
        output << "objekt ist an der Stelle " << result;
        cmd_echo(client,msg,output.str());
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