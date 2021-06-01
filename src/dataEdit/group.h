#ifndef TIPPSPIEL_DATA_EDIT_GROUP_H
#define TIPPSPIEL_DATA_EDIT_GROUP_H

#include <tobilib/database/database.h>

namespace data_edit {

    tobilib::Database::Cluster group_create(tobilib::Database::Cluster admin, std::string name);
    void group_rename(tobilib::Database::Cluster group, std::string name);
    void group_refresh_token(tobilib::Database::Cluster group);
    void group_add_user(tobilib::Database::Cluster group, tobilib::Database::Cluster user);
    void group_remove_user(tobilib::Database::Cluster group, tobilib::Database::Cluster user);

}

#endif