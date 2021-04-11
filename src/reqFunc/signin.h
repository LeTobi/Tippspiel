#ifndef TIPPSPIEL_REQ_FUNCTION_SIGNIN_H
#define TIPPSPIEL_REQ_FUNCTION_SIGNIN_H

#include "../session.h"

namespace msg_handler {

    void signin(Session&, tobilib::h2rfp::Message&);
    
}

#endif