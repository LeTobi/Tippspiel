#ifndef TIPPSPIEL_GLOBAL_H
#define TIPPSPIEL_GLOBAL_H

#include <tobilib/ssl/h2rfp.h>
#include <tobilib/database/database.h>
#include "enums.h"
#include "components/email.h"
#include "components/virtualdb.h"

using namespace tobilib;

class Client{
public:
    Client();

    h2rfp::WSS_Endpoint endpoint;
    Database::Cluster user;
    unsigned int emailtask = 0;
    
    static network::Acceptor acceptor;
};

class GlobalData{
public:
    GlobalData();

    Logger log;
    Database database;
    VirtualDB virtualdb;
    TokenEmail emails;
    std::vector<Client> clients;
};

extern GlobalData* data;

#endif