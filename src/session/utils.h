#ifndef TIPPSPIEL_REQUEST_UTILS_H
#define TIPPSPIEL_REQUEST_UTILS_H

#include <tobilib/database/database.h>
#include "../global.h"

tobilib::Database::Cluster check_token(std::string);
std::string make_new_token(tobilib::Database::Cluster);
bool check_login(Client&, h2rfp::Message);
bool check_permission(Client&, h2rfp::Message, const std::string&);
bool check_parameter(Client&, h2rfp::Message, const std::string&);


h2rfp::JSObject make_result();
h2rfp::JSObject make_user_error(int);
void return_result(Client&, h2rfp::Message, h2rfp::JSObject);
void return_client_error(Client&, h2rfp::Message, const std::string&);
void return_server_error(Client&, h2rfp::Message, const std::string&);

#endif