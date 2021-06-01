#ifndef TIPPSPIEL_MSG_CREATION_LOCATION_H
#define TIPPSPIEL_MSG_CREATION_LOCATION_H

#include <tobilib/protocols/h2rfp.h>
#include "../session.h"
#include "../misc/msgID.h"

namespace msg_creation {
namespace detail {

    bool location_list_individual(Session&, const MsgID&);
    tobilib::h2rfp::JSObject location_list(Session&, const MsgID&);

}
}

#endif