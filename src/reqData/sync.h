#ifndef TIPPSPIEL_REQDATA_SYNC_H
#define TIPPSPIEL_REQDATA_SYNC_H

#include "../session.h"

namespace msg_handler {

    void sync(Session&, const tobilib::h2rfp::Message&);
    void inform_identity(Session&, const tobilib::h2rfp::Message&);

}

#endif