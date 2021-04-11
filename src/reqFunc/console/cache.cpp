#include "cache.h"
#include "util.h"
#include "../../misc/response_util.h"
#include "../../msgCache.h"
#include "../../msgTracking.h"
#include "../../main-data.h"

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
        output << maindata->cache.stat_hits << "/" << maindata->cache.stat_requests << " (" << percent << "%) hits\n";
        output << maindata->cache.size() << "/" << MsgCache::CACHE_SIZE << " belegt";
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
    else if (cmd=="update")
    {
        std::string clustername;
        unsigned int index;
        input >> clustername >> index;
        if (!input) {
            cmd_echo(session,msg,"erwarte cluster und index");
            return;
        }
        if (maindata->storage.getType(clustername) == Database::ClusterType::invalid) {
            cmd_echo(session,msg,"Clustertype nicht gefunden");
            return;
        }
        Database::Cluster target = maindata->storage.list(clustername)[index];
        if (target.is_null()) {
            cmd_echo(session,msg,"Der Index existiert nicht");
            return;
        }
        global_message_update(target,WAIT_SHORT);
        cmd_echo(session,msg,"Update eingereiht");
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