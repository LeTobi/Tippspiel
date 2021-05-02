#ifndef TIPPSPIEL_REQ_FUNCTION_GROUP_H
#define TIPPSPIEL_REQ_FUNCTION_GROUP_H

#include "../session.h"

namespace msg_handler {

    void group_create     (Session&,const tobilib::h2rfp::Message&);
    void group_rename     (Session&,const tobilib::h2rfp::Message&);
    void group_join       (Session&,const tobilib::h2rfp::Message&);
    void group_reset_token(Session&,const tobilib::h2rfp::Message&);
    void group_leave      (Session&,const tobilib::h2rfp::Message&);

}

#endif