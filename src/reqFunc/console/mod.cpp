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
    else
    {
        cmd_echo(session,msg,"unbekannter befehl fuer mod");
        return;
    }
}