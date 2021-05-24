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

enum {
    WORK_BUSY,
    WORK_BACKGROUND,
    WORK_STANDBY
};

class MainData
{
public:
    MainData();
    MainData(const MainData&) = delete;
    ~MainData();

    void init(int);
    void tick();
    void request_work(int);

    tobilib::Logger log;
    tobilib::Database storage;
    std::vector<Session> sessions;
    int workstate = WORK_STANDBY;

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