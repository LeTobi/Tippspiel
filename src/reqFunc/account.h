#ifndef TIPPSPIEL_REQ_FUNC_ACCOUNT_H
#define TIPPSPIEL_REQ_FUNC_ACCOUNT_H

#include "../session.h"

namespace msg_handler {

    void make_new_user(Session&, const tobilib::h2rfp::Message&);
    void restore_token(Session&, const tobilib::h2rfp::Message&);
    void setlang(Session&, const tobilib::h2rfp::Message&);

}

#endif