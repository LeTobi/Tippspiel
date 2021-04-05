#ifndef TIPPSPIEL_DATAEDIT_USER_H
#define TIPPSPIEL_DATAEDIT_USER_H

#include "../misc/time.h"
#include <tobilib/database/database.h>

namespace data_edit {

    void set_user_sync(tobilib::Database::Cluster, Time);

}

#endif