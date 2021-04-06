#include "all.h"
#include "event.h"
#include "eventTipp.h"
#include "game.h"
#include "gameTipp.h"
#include "player.h"
#include "team.h"
#include "user.h"
#include "filters.h"
#include "location.h"

using namespace tobilib;
using namespace h2rfp;

bool msg_creation::is_individual(Session& session, const MsgID& id)
{
    switch (id.type)
    {
    case MsgType::event:
        return detail::event_is_individual(session,id);
    case MsgType::eventTipp:
        return detail::eventtipp_is_individual(session,id);
    case MsgType::game:
        return detail::game_is_individual(session,id);
    case MsgType::gameTipp:
        return detail::gametipp_is_individual(session,id);
    case MsgType::player:
        return detail::player_is_individual(session,id);
    case MsgType::team:
        return detail::team_is_individual(session,id);
    case MsgType::user:
        return detail::user_is_individual(session,id);
    case MsgType::hotGames:
        return detail::hot_games_individual(session,id);
    case MsgType::locations:
        return detail::location_list_individual(session,id);
    case MsgType::gamePlayers:
        return detail::suggest_players_individual(session,id);
    case MsgType::none:
        break;
    }
    throw Exception("Unbekannter MsgType","msg_creation::is_individual()");
}

JSObject msg_creation::make_msg(Session& session, const MsgID& id)
{
    switch(id.type)
    {
    case MsgType::event:
        return detail::event_make_msg(session,id);
    case MsgType::eventTipp:
        return detail::eventtipp_make_msg(session,id);
    case MsgType::game:
        return detail::game_make_msg(session,id);
    case MsgType::gameTipp:
        return detail::gametipp_make_msg(session,id);
    case MsgType::player:
        return detail::player_make_msg(session,id);
    case MsgType::team:
        return detail::team_make_msg(session,id);
    case MsgType::user:
        return detail::user_make_msg(session,id);
    case MsgType::hotGames:
        return detail::hot_games(session,id);
    case MsgType::locations:
        return detail::location_list(session,id);
    case MsgType::gamePlayers:
        return detail::suggest_players(session,id);
    case MsgType::none:
        break;
    }
    throw Exception("Unbekannter MsgType","msg_creation::make_msg()");
}