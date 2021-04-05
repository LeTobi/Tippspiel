#ifndef TIPPSPIEL_MSG_CREATION_GAME_H
#define TIPPSPIEL_MSG_CREATION_GAME_H

#include <tobilib/protocols/h2rfp.h>
#include "../session.h"
#include "../misc/msgID.h"

namespace msg_creation {
namespace detail {

bool game_is_individual(Session&, const MsgID&);
tobilib::h2rfp::JSObject game_make_msg(Session&, const MsgID&);

}
}

#endif