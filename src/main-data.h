#ifndef TIPPSPIEL_MAINDATA_H
#define TIPPSPIEL_MAINDATA_H

#include <tobilib/general/exception.hpp>
#include <tobilib/database/database.h>
#include <tobilib/network/acceptor.h>
#include <fstream>

#include "session.h"

class MsgCache;
class UpdateTracker;
class DatabaseFilters;
class ServerTasks;
class ServerActions;

class MainData
{
public:
    MainData();
    MainData(const MainData&) = delete;
    ~MainData();

    void init(int);
    void tick();

    tobilib::Logger log;
    tobilib::Database storage;
    std::vector<Session> sessions;

    MsgCache& cache;
    UpdateTracker& updateTracker;
    DatabaseFilters& filters;
    ServerTasks& tasks;
    ServerActions& actions;
    

    tobilib::network::Acceptor* acceptor = nullptr; // only use in Session constructor
private:
    std::fstream logfile;
    
};

extern MainData* maindata;

#endif