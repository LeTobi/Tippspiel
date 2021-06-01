#include "msgDistributor.h"
#include "../main-data.h"

using namespace tobilib;

void MsgDistributor::simple_no_response_to_all(const std::string& name) const
{
    for (Session& session: maindata->sessions)
    {
        if (session.status == Session::Status::active)
            session.client.notify(name);
    }
}