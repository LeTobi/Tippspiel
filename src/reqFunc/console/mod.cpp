#include "mod.h"
#include "util.h"
#include "../../dataEdit/game.h"
#include "../../main-data.h"
#include <sstream>

using namespace tobilib;
using namespace h2rfp;

void cmd_mod(Session& session, const Message& msg,const std::string& in)
{
    std::stringstream input (in);
    std::string cmd;
    input >> cmd;

    if (cmd=="gameval")
    {
        int index;
        input >> index;
        Database::Cluster game = maindata->storage.list("Game")[index];
        if (game.is_null())
        {
            cmd_echo(session,msg,"Zu diesem Index existiert kein Spiel");
            return;
        }
        data_edit::game_evaluate(game);
        cmd_echo(session,msg,"Das Spiel wurde ausgewertet");
        return;
    }
    if (cmd=="erase")
    {
        std::string table;
        int index;
        input >> table >> index;
        if (!input)
        {
            cmd_echo(session,msg,"erwarte tabelle und index");
            return;
        }
        Database::ClusterType type = maindata->storage.getType(table);
        if (type == Database::ClusterType::invalid)
        {
            cmd_echo(session,msg,"unbekannte Tabelle");
            return;
        }
        Database::Cluster cluster = maindata->storage.list(type.name)[index];
        if (cluster.is_null())
        {
            cmd_echo(session,msg,"Der eintrag in der Tabelle existiert nicht.");
            return;
        }

        if (type.name == "Game")
        {
            data_edit::delete_game(cluster);
            cmd_echo(session,msg,"Das Spiel wurde entfernt");
            return;
        }
        else
        {
            cmd_echo(session,msg,"Aus dieser Tabelle kann noch nicht entfernt werden.");
            return;
        }
    }
    else
    {
        cmd_echo(session,msg,"unbekannter befehl fuer mod");
        return;
    }
}