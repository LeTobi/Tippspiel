#ifndef TIPPSPIEL_EVENTHANDLER_H
#define TIPPSPIEL_EVENTHANDLER_H

#include "global.h"

void signup(Client&, h2rfp::Message);
void signup_complete(Client&, TokenEmail::Result);
void signin(Client&, h2rfp::Message);
void inform_identity(Client&, h2rfp::Message);
void console_input(Client&, h2rfp::Message);
void serve_data(Client&, h2rfp::Message);

#endif