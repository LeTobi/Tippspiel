#include "pong.h"
#include <tobilib/ssl/h2rfp.h>
#include "../session.h"

using namespace tobilib;
using namespace network;
using namespace h2rfp;

void SessionPong::tick()
{
    response.pull(session->client.responses);
    if (response.is_received())
        response.dismiss();
    if (session->client.status()==EndpointStatus::closed)
        response.dismiss();
}

bool SessionPong::pending()
{
    return response.is_requested();
}

void SessionPong::do_pong()
{
    if (!response.is_requested())
        response = session->client.request("Ping");
}