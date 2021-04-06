#ifndef TIPPSPIEL_REQ_FUNC_GAME_H
#define TIPPSPIEL_REQ_FUNC_GAME_H

#include "../session.h"

namespace msg_handler {

    void game_tipp(Session&, tobilib::h2rfp::Message&);
    void game_announce(Session&, tobilib::h2rfp::Message&);

}

#endif