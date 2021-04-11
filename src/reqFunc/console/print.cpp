#include "print.h"
#include "util.h"
#include "../../misc/response_util.h"
#include "../../misc/config.h"
#include "../../main-data.h"
#include <tobilib/database/manipulation/export.h>

using namespace tobilib;
using namespace h2rfp;

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