#ifndef TIPPSPIEL_MISC_RESPONSE_UTIL_H
#define TIPPSPIEL_MISC_RESPONSE_UTIL_H

#include "../session.h"
#include "enums.h"

bool check_login(Session&, tobilib::h2rfp::Message);
bool check_permission(Session&, tobilib::h2rfp::Message, const std::string&);
bool check_parameter(Session&, tobilib::h2rfp::Message, const std::string&);


tobilib::h2rfp::JSObject make_result();
tobilib::h2rfp::JSObject make_user_error(int);

void return_result(Session&, tobilib::h2rfp::Message, tobilib::h2rfp::JSObject);
void return_result(Session&, unsigned int, tobilib::h2rfp::JSObject);

void return_client_error(Session&, tobilib::h2rfp::Message, const std::string&);
void return_client_error(Session&, unsigned int, const std::string&);

void return_server_error(Session&, tobilib::h2rfp::Message, const std::string&);
void return_server_error(Session&, unsigned int, const std::string&);

#endif