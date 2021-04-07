#ifndef TIPPSPIEL_REQ_FUNC_CONSOLE_MAIN_H
#define TIPPSPIEL_REQ_FUNC_CONSOLE_MAIN_H

#include "../../session.h"

void cmd_echo(Session&, const tobilib::h2rfp::Message&, const std::string&);

void cmd_access(Session&, const tobilib::h2rfp::Message&, const std::string&);
void cmd_print(Session&, const tobilib::h2rfp::Message&, const std::string&);
void cmd_cache(Session&, const tobilib::h2rfp::Message&, const std::string&);
void cmd_guard(Session&, const tobilib::h2rfp::Message&, const std::string&);

#endif