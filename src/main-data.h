#ifndef TIPPSPIEL_MAINDATA_H
#define TIPPSPIEL_MAINDATA_H

#include <tobilib/general/exception.hpp>
#include <tobilib/database/database.h>
#include <tobilib/network/acceptor.h>
#include <fstream>

#include "session.h"
#include "msgCache.h"
#include "updateTracker.h"
#include "tasks/all.h"
#include "filters/all.h"
#include "actions/all.h"

class MainData{
public:
    void init(int);
    ~MainData();

    tobilib::Logger log;
    tobilib::Database storage;
    std::vector<Session> sessions;
    MsgCache cache;
    UpdateTracker updateTracker;

    tasks::Registration registration;
    filters::GameTimeline gametimeline;

    tobilib::network::Acceptor* acceptor = nullptr; // only use in Session constructor
private:
    std::fstream logfile;
    
};

extern MainData* maindata;

#endif