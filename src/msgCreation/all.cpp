#include "all.h"
#include "event.h"
#include "eventTipp.h"
#include "game.h"
#include "gameTipp.h"
#include "player.h"
#include "team.h"
#include "user.h"
#include "filters.h"


using namespace tobilib;
using namespace h2rfp;

bool msg_creation::is_individual(Session& session, const MsgID& id)
{
    if (id.type == MsgType::event)
        return detail::event_is_individual(session,id);
    if (id.type == MsgType::eventTipp)
        return detail::eventtipp_is_individual(session,id);
    if (id.type == MsgType::game)
        return detail::game_is_individual(session,id);
    if (id.type == MsgType::gameTipp)
        return detail::gametipp_is_individual(session,id);
    if (id.type == MsgType::player)
        return detail::player_is_individual(session,id);
    if (id.type == MsgType::team)
        return detail::team_is_individual(session,id);
    if (id.type == MsgType::user)
        return detail::user_is_individual(session,id);
    if (id.type == MsgType::hotGames)
        return detail::hot_games_individual(session,id);
    throw Exception("Unbekannter MsgType","msg_creation::is_individual()");
}

JSObject msg_creation::make_msg(Session& session, const MsgID& id)
{
    if (id.type == MsgType::event)
        return detail::event_make_msg(session,id);
    if (id.type == MsgType::eventTipp)
        return detail::eventtipp_make_msg(session,id);
    if (id.type == MsgType::game)
        return detail::game_make_msg(session,id);
    if (id.type == MsgType::gameTipp)
        return detail::gametipp_make_msg(session,id);
    if (id.type == MsgType::player)
        return detail::player_make_msg(session,id);
    if (id.type == MsgType::team)
        return detail::team_make_msg(session,id);
    if (id.type == MsgType::user)
        return detail::user_make_msg(session,id);
    if (id.type == MsgType::hotGames)
        return detail::hot_games(session,id);
    throw Exception("Unbekannter MsgType","msg_creation::make_msg()");
}