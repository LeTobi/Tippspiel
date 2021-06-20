#ifndef TIPPSPIEL_DATAEDIT_EVENT_H
#define TIPPSPIEL_DATAEDIT_EVENT_H

#include <tobilib/database/database.h>

namespace data_edit
{
    tobilib::Database::Cluster get_event_tipp(
        tobilib::Database::Cluster user,
        tobilib::Database::Cluster event,
        bool force);
    void set_event_tipp(
        tobilib::Database::Cluster tipp,
        tobilib::Database::Cluster winnerteam,
        tobilib::Database::Cluster topscorer
    );
    void report_event(
        tobilib::Database::Cluster event,
        tobilib::Database::Cluster user,
        tobilib::Database::Cluster winnerteam,
        tobilib::Database::Cluster topscorer
    );
    void event_evaluate(
        tobilib::Database::Cluster event
    );
}

#endif