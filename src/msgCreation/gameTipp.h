#ifndef TIPPSPIEL_MSG_CREATION_GAMETIPP_H
#define TIPPSPIEL_MSG_CREATION_GAMETIPP_H

#include <tobilib/protocols/h2rfp.h>
#include "../session.h"
#include "../misc/msgID.h"

namespace msg_creation {
namespace detail {

bool gametipp_is_individual(Session&, const MsgID&);
tobilib::h2rfp::JSObject gametipp_make_msg(Session&, const MsgID&);

}
}

#endif