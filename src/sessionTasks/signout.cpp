#include "signout.h"
#include "../session.h"
#include <tobilib/ssl/h2rfp.h>
#include "all.h"
#include "../misc/response_util.h"

using namespace tobilib;

void SessionSignout::tick()
{
    if (session->client.status() != network::EndpointStatus::connected)
        signout_in_process = false;
    if (!signout_in_process)
        return;
    if (session->tasks.pending())
        return;

    session->user = Database::Cluster();
    signout_in_process = false;
    h2rfp::JSObject answer = make_result();
    session->client.respond(msg_id,answer);
}

bool SessionSignout::pending() const
{
    return false;
}

void SessionSignout::signout(unsigned int rid)
{
    msg_id = rid;
    signout_in_process = true;
    tick();
}
