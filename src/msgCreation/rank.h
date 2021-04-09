#ifndef TIPPSPIEL_MSG_CREATION_RANK_H
#define TIPPSPIEL_MSG_CREATION_RANK_H

#include <tobilib/protocols/h2rfp.h>
#include "../session.h"
#include "../misc/msgID.h"

namespace msg_creation {
namespace detail {

    bool event_rank_individual(Session&, MsgID);
    tobilib::h2rfp::JSObject event_rank_msg(Session&, MsgID);

}
}

#endif