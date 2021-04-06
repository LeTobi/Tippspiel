#ifndef TIPPSPIEL_REQ_DATA_SUGGESTIONS_H
#define TIPPSPIEL_REQ_DATA_SUGGESTIONS_H

#include "../session.h"

namespace msg_handler {

    void suggest_locations(Session&, const tobilib::h2rfp::Message&);
    void suggest_players(Session&, const tobilib::h2rfp::Message&);

}

#endif