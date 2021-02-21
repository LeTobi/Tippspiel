#ifndef TIPPSPIEL_CONSOLEINPUT_H
#define TIPPSPIEL_CONSOLEINPUT_H

#include "../global.h"

void cmd_echo(Client&, h2rfp::Message, const std::string&);

void cmd_access(Client&, h2rfp::Message, const std::string&);
void cmd_print(Client&, h2rfp::Message, const std::string&);
void cmd_cache(Client&, h2rfp::Message, const std::string&);
void cmd_guard(Client&, h2rfp::Message, const std::string&);

#endif