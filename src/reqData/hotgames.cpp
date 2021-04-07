#include "hotgames.h"
#include "../main-data.h"
#include "../msgCache.h"
#include "../misc/response_util.h"

using namespace tobilib;
using namespace h2rfp;

void msg_handler::hot_games(Session& session, const tobilib::h2rfp::Message& msg)
{
    JSObject answer = make_result();
    answer.put_child("data", maindata->cache.get_data(session, MsgType::hotGames));
    return_result(session,msg,answer);
}