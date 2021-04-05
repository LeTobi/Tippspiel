#ifndef TIPPSPIEL_REQ_FUNCTION_PLAYER_H
#define TIPPSPIEL_REQ_FUNCTION_PLAYER_H

#include "../session.h"

namespace msg_handler {

    void create_player(Session&,const tobilib::h2rfp::Message&);

}

#endif