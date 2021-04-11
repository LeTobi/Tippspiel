#include "access.h"
#include "util.h"
#include "../../misc/response_util.h"
#include <tobilib/database/manipulation/commands.h>
#include "../../main-data.h"

using namespace tobilib;
using namespace h2rfp;

void cmd_access(Session& session, const Message& msg, const std::string& cmd)
{
    cmd_echo(session, msg, database_tools::command(maindata->storage, cmd) );
}