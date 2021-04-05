#include "player.h"
#include "../misc/response_util.h"
#include "../main-data.h"
#include "../dataEdit/player.h"

using namespace tobilib;

void msg_handler::create_player(Session& session, const h2rfp::Message& msg)
{
   if (!check_login(session,msg) ||
        !check_parameter(session,msg,"name") ||
        !check_parameter(session,msg,"team"))
        return;
    Database::Cluster team = maindata->storage.list("Team")[msg.data.get("team",0)];
    std::string name = msg.data.get("name","");
    if (team.is_null()) {
        return_client_error(session,msg,"Das Team ist unbekannt");
        return;
    }
    if (name.size() < 4) {
        h2rfp::JSObject result = make_user_error(ERROR_INVALID_NAME);
        result.put("data.name",name);
        return_result(session,msg,result);
        return;
    }
    
    Database::Cluster player = data_edit::create_player(session.user,name,team);

    h2rfp::JSObject result = make_result();
    result.put("data.id", player.index());
    return_result(session,msg,result);
    return;
}