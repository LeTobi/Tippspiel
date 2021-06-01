#ifndef TIPPSPIEL_MSG_CREATION_PLAYER_H
#define TIPPSPIEL_MSG_CREATION_PLAYER_H

#include <tobilib/protocols/h2rfp.h>
#include "../session.h"
#include "../misc/msgID.h"

namespace msg_creation {
namespace detail {

bool player_is_individual(Session&, const MsgID&);
tobilib::h2rfp::JSObject player_make_msg(Session&, const MsgID&);

}
}

#endif