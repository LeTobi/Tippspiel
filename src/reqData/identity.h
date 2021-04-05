#ifndef TIPPSPIEL_REQ_DATA_IDENTITY_H
#define TIPPSPIEL_REQ_DATA_IDENTITY_H

#include "../session.h"

namespace msg_handler {

    void inform_identity(Session&, const tobilib::h2rfp::Message&);

}

#endif