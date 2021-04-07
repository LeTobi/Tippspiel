#include "sync.h"
#include "../misc/response_util.h"
#include "../misc/msgID.h"
#include "../main-data.h"
#include "../msgCache.h"
#include <map>

using namespace tobilib;

const std::map<std::string,MsgType> msgTypes {
    std::make_pair("User",MsgType::user),
    std::make_pair("Group",MsgType::group),
    std::make_pair("Player",MsgType::player),
    std::make_pair("Team",MsgType::team),
    std::make_pair("Event",MsgType::event),
    std::make_pair("Game",MsgType::game),
    std::make_pair("EventTipp",MsgType::eventTipp),
    std::make_pair("GameTipp",MsgType::gameTipp)
};

void msg_handler::sync(Session& session, const tobilib::h2rfp::Message& msg)
{
    std::string table = msg.data.get("table","");
    if (msgTypes.count(table)==0)
    {
        return_client_error(session,msg,std::string("Tabelle nicht bekannt: ")+table);
        return;
    }

    h2rfp::JSObject ids;
    try {
        ids = msg.data.get_child("ids");
    } catch (boost::property_tree::ptree_error& e) {
        return_client_error(session,msg,"Keine Ids angegeben");
        return;
    }

    h2rfp::JSObject outlist;
    for (auto& item: ids)
    {
        unsigned int id = item.second.get_value(0u);
        if (id==0)
        {
            return_client_error(session,msg,"Eine Id stimmt nicht");
            return;
        }
        MsgID msgid (msgTypes.at(table),id);
        outlist.push_back(std::make_pair("",maindata->cache.get_data(session,msgid)));
    }

    h2rfp::JSObject output = make_result();
    output.put_child("data", outlist);
    return_result(session,msg,output);
}