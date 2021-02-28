#include "global.h"

using namespace tobilib;

GlobalData* data;
TokenEmail* emails;

network::Acceptor Client::acceptor (15320);

Client::Client(): endpoint(acceptor)
{
    endpoint.log.parent = &data->log;
}

GlobalData::GlobalData(): clients(100), database("../../data/")
{
    for (int i=0;i<clients.size();i++)
    {
        clients[i].endpoint.log.prefix = std::string("endpoint ")+std::to_string(i)+": ";
        clients[i].endpoint.options.handshake_timeout = 5;
        clients[i].endpoint.options.read_timeout      = 10;
        clients[i].endpoint.options.inactive_warning  = 5;
        clients[i].endpoint.options.send_timeout      = 0;
        clients[i].endpoint.options.connect_timeout   = 0;
        clients[i].endpoint.options.close_timeout     = 2;
        clients[i].endpoint.connect();
    }
    database.log.parent = &log;
}