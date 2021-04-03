#ifndef TIPPSPIEL_GLOBAL_H
#define TIPPSPIEL_GLOBAL_H

#include <tobilib/ssl/h2rfp.h>
#include <tobilib/database/database.h>
#include <fstream>
#include "enums.h"
#include "components/email.h"
#include "components/virtualdb.h"
#include "components/gameobserver.h"

using namespace tobilib;

class Client{
public:
    struct Update {
        Time time;
        h2rfp::Response response;
    };

    Client();

    h2rfp::WSS_Endpoint endpoint;
    Database::Cluster user;
    Task emailtask = 0;
    std::vector<Update> updates;
    h2rfp::Response pong;
    
    static network::Acceptor* acceptor;
};

class GlobalData{
public:
    GlobalData();
    void init();
    ~GlobalData();

    Logger log;
    std::fstream logfile;
    Database database;
    VirtualDB virtualdb;
    TokenEmail emails;
    GameObserver observer;
    std::vector<Client> clients;
};

extern GlobalData* data;

#endif