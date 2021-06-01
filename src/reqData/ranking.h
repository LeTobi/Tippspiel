#ifndef TIPPSPIEL_REQ_DATA_RANKING_H
#define TIPPSPIEL_REQ_DATA_RANKING_H

#include "../session.h"

namespace msg_handler {

    void get_event_ranking(Session&,const tobilib::h2rfp::Message&);

}

#endif
