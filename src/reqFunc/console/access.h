#ifndef TIPPSPIEL_REQ_FUNC_CONSOLE_ACCESS_H
#define TIPPSPIEL_REQ_FUNC_CONSOLE_ACCESS_H

#include "../../session.h"

void cmd_access(Session&, const tobilib::h2rfp::Message&, const std::string&);

#endif