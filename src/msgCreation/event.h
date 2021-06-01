#ifndef TIPPSPIEL_MSG_CREATION_EVENT_H
#define TIPPSPIEL_MSG_CREATION_EVENT_H

#include <tobilib/protocols/h2rfp.h>
#include "../session.h"
#include "../misc/msgID.h"

namespace msg_creation {
namespace detail {

bool event_is_individual(Session&, const MsgID&);
tobilib::h2rfp::JSObject event_make_msg(Session&, const MsgID&);

}
}

#endif