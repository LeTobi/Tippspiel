#include "util.h"
#include "../../misc/response_util.h"

using namespace tobilib;
using namespace h2rfp;

void cmd_echo(Session& session, const Message& msg, const std::string& txt)
{
    h2rfp::JSObject answer = make_result();
    answer.put("data.text",txt);
    return_result(session,msg,answer);
}