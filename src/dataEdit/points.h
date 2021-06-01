#ifndef TIPPSPIEL_DATAEDIT_POINTS_H
#define TIPPSPIEL_DATAEDIT_POINTS_H

#include <tobilib/database/database.h>

namespace data_edit {

    tobilib::Database::Cluster get_rank(tobilib::Database::Cluster user, tobilib::Database::Cluster event, bool force);
    void tipp_set_result(
        tobilib::Database::Cluster tipp,
        int kategorie,
        int goals,
        bool penaltyBonus
        );

}

#endif