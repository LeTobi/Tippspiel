#ifndef TIPPSPIEL_DATAEDIT_USER_H
#define TIPPSPIEL_DATAEDIT_USER_H

#include "../misc/time.h"
#include <tobilib/database/database.h>

namespace data_edit {

    tobilib::Database::Cluster create_user(const std::string& name, const std::string& email);

    void set_user_token(tobilib::Database::Cluster,const std::string&);
    void set_user_last_login(tobilib::Database::Cluster);

    // Synchronisierungs mechanismus
    void set_user_sync(tobilib::Database::Cluster, Time);

}

#endif