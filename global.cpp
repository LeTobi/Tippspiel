#include "global.h"

using namespace tobilib;

GlobalData* data;
TokenEmail* emails;

network::Acceptor Client::acceptor (15320);

Client::Client(): endpoint(acceptor)
{
    endpoint.log.parent = &data->log;
}

GlobalData::GlobalData(): clients(100), database("../data/")
{
    for (int i=0;i<clients.size();i++)
    {
        clients[i].endpoint.log.prefix = std::string("endpoint ")+std::to_string(i)+": ";
        clients[i].endpoint.connect();
    }
    database.log.parent = &log;
}