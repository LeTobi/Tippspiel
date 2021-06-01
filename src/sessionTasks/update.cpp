#include "update.h"
#include "../session.h"
#include "../dataEdit/user.h"

using namespace tobilib;
using namespace h2rfp;

void SessionUpdate::tick()
{
    auto it = requests.rbegin();
    while (it!=requests.rend())
    {
        it->response.pull(session->client.responses);
        if (!it->response.is_received()) {
            ++it;
            continue;
        }
        data_edit::set_user_sync(session->user,it->synctime);
        ++it;
        requests.erase(it.base());
    }
    if (session->client.status()==EndpointStatus::closed)
        requests.clear();
}

bool SessionUpdate::pending()
{
    return !requests.empty();
}

void SessionUpdate::send_update(const JSObject& data, Time stime)
{
    requests.emplace_back();
    requests.back().synctime = stime;
    requests.back().response = session->client.request("Update",data);
}