#ifndef TIPPSPIEL_DATAEDIT_PLAYER_H
#define TIPPSPIEL_DATAEDIT_PLAYER_H

#include <tobilib/database/database.h>

namespace data_edit {
    tobilib::Database::Cluster create_player(
        tobilib::Database::Cluster user,
        std::string name,
        tobilib::Database::Cluster team
    );
}

#endif