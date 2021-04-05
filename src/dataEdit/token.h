#ifndef TIPPSPIEL_DATAEDIT_TOKEN_H
#define TIPPSPIEL_DATAEDIT_TOKEN_H

#include <tobilib/database/database.h>

namespace data_edit {

    std::string make_new_token(tobilib::Database::Cluster);

    tobilib::Database::Cluster check_token(std::string);

}

#endif