#ifndef TIPPSPIEL_DATAEDIT_USER_H
#define TIPPSPIEL_DATAEDIT_USER_H

#include "../misc/time.h"
#include <tobilib/database/database.h>

namespace data_edit {

    tobilib::Database::Cluster create_user(const std::string& name, const std::string& email);
    void delete_user(tobilib::Database::Cluster user); // nur wenn der nutzer noch nie eingeloggt war

    void set_user_token(tobilib::Database::Cluster,const std::string&);
    void set_user_last_login(tobilib::Database::Cluster);
    void set_user_ban(tobilib::Database::Cluster user, bool ban);
    void set_user_lastrecovery(tobilib::Database::Cluster user, Time t);

    // Synchronisierungs mechanismus
    void set_user_sync(tobilib::Database::Cluster, Time);

}

#endif