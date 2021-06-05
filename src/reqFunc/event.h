#ifndef TIPPSPIEL_REQ_FUNC_EVENT_H
#define TIPPSPIEL_REQ_FUNC_EVENT_H

#include "../session.h"

namespace msg_handler {

    void event_tipp(Session&, tobilib::h2rfp::Message&);

}

#endif