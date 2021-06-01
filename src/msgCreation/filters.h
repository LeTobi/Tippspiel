#ifndef TIPPSPIEL_MSG_GENERATION_FILTERS_H
#define TIPPSPIEL_MSG_GENERATION_FILTERS_H

#include <tobilib/protocols/h2rfp.h>
#include "../session.h"
#include "../misc/msgID.h"

namespace msg_creation {
namespace detail {

    bool hot_games_individual(Session&, const MsgID&);
    tobilib::h2rfp::JSObject hot_games(Session&, const MsgID&);

}
}

#endif