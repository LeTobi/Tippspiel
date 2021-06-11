#ifndef TIPPSPIEL_REQ_FUNCTION_PUSH_H
#define TIPPSPIEL_REQ_FUNCTION_PUSH_H

#include "../session.h"

namespace msg_handler {

    void push_enable(Session&, const tobilib::h2rfp::Message&);
    void push_disable(Session&, const tobilib::h2rfp::Message&);
}

#endif