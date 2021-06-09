#include "msgDistributor.h"
#include "../main-data.h"

using namespace tobilib;
using namespace h2rfp;

void MsgDistributor::simple_no_response_to_all(const std::string& name) const
{
    for (Session& session: maindata->sessions)
    {
        if (session.status == Session::Status::active)
            session.client.notify(name);
    }
}

void MsgDistributor::data_no_response_to_all(const std::string& name, const JSObject& data) const
{
    for (Session& session: maindata->sessions)
    {
        if (session.status == Session::Status::active)
            session.client.notify(name,data);
    }
}