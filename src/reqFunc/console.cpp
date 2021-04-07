#include "console.h"
#include "console/main.h"
#include "../misc/response_util.h"

using namespace tobilib;

void msg_handler::console_input(Session& session, const h2rfp::Message& msg)
{
    if (!check_login(session,msg) ||
        !check_permission(session,msg,"perm_console") )
        return;

    std::string line = msg.data.get("cmd","");
    session.log << "Command: " << line << std::endl;
    size_t space = line.find(' ');
    if (space==std::string::npos)
        space = line.size();
    std::string cmd = line.substr(0,space);
    std::string rest = line.substr(space);
    
    if (cmd == "access") {
        cmd_access(session,msg,rest);
    }
    else if (cmd == "print") {
        cmd_print(session,msg,rest);
    }
    else if (cmd == "cache") {
        cmd_cache(session,msg,rest);
    }
    else if (cmd=="guard") {
        cmd_guard(session,msg,rest);
    }
    else {
        cmd_echo(session,msg,std::string("unbekannter befehl: ")+cmd);
    }

}