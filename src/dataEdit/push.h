#ifndef TIPPSPIEL_DATAEDIT_PUSH_H
#define TIPPSPIEL_DATAEDIT_PUSH_H

#include <tobilib/database/database.h>

namespace data_edit {

    tobilib::Database::Cluster get_subscription(const std::string& endpoint);

    tobilib::Database::Cluster add_push(
        tobilib::Database::Cluster user,
        const std::string& endpoint,
        const std::string& p256dh,
        const std::string& auth);

    bool remove_push(const std::string& endpoint);

}

#endif