#ifndef TIPPSPIEL_REQ_DATA_HOTGAMES_H
#define TIPPSPIEL_REQ_DATA_HOTGAMES_H

#include "../session.h"

namespace msg_handler {

    void hot_games(Session&, const tobilib::h2rfp::Message&);
}

#endif