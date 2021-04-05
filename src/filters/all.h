#ifndef TIPPSPIEL_FILTERS_ALL_H
#define TIPPSPIEL_FILTERS_ALL_H

#include <tobilib/database/database.h>
#include "timeline_game.h"

namespace filters {

    void tick_all();
    void update(tobilib::Database::Cluster);
    void remove(tobilib::Database::Cluster);

}

#endif