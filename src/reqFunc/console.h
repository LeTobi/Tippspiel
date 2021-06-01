#ifndef TIPPSPIEL_REQ_FUNC_CONSOLE_H
#define TIPPSPIEL_REQ_FUNC_CONSOLE_H

#include "../session.h"

namespace msg_handler {

    void console_input(Session&, const tobilib::h2rfp::Message&);

}

#endif