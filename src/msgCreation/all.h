#ifndef TIPPSPIEL_MSG_CREATION_H
#define TIPPSPIEL_MSG_CREATION_H

#include <tobilib/protocols/h2rfp.h>
#include "../misc/msgID.h"
#include "../session.h"

namespace msg_creation
{
    bool is_individual(Session&, const MsgID&);
    tobilib::h2rfp::JSObject make_msg(Session&, const MsgID&);
}

#endif