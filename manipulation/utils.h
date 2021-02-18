#ifndef TIPPSPIEL_REQUEST_UTILS_H
#define TIPPSPIEL_REQUEST_UTILS_H

#include <tobilib/database/database.h>
#include "../global.h"

tobilib::Database::Cluster check_token(std::string);
std::string make_new_token(tobilib::Database::Cluster);
bool check_login(Client&, unsigned int = 0);
bool check_permission(const std::string&, Client&, unsigned int = 0);

#endif