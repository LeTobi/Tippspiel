#include "main-data.h"
#include <tobilib/ssl/network.h>
#include "msgCache.h"
#include "updateTracker.h"
#include "tasks/all.h"
#include "filters/all.h"
#include "actions/all.h"

using namespace tobilib;

MainData::MainData():
    cache(*new MsgCache()),
    updateTracker(*new UpdateTracker()),
    filters(*new DatabaseFilters),
    tasks(*new ServerTasks()),
    actions(*new ServerActions())
{ }

MainData::~MainData()
{
    delete &cache;
    delete &updateTracker;
    delete &tasks;
    delete &filters;
    delete &actions;
    if (acceptor!=nullptr)
        delete acceptor;
}

void MainData::init(int port) {
    updateTracker.log.parent = &log;

    logfile.open("logfile",logfile.out | logfile.app);
    if (logfile.good()) {
        log.print_time = true;
        log.dump = &logfile;
    } else {
        log.dump = &std::cerr;
        throw Exception("Kein zugriff auf die Logdatei","GlobalData::init()");
    }

    if (acceptor != nullptr)
        throw Exception("Mehrfache initialisierung, Programmierfehler","GlobalData::init()");
    acceptor = new network::Acceptor (port);


    storage.log.parent = &log;
    storage.setPath("../../data/");
    storage.init();
    storage.open();

    if (!storage.is_good())
        throw Exception("Tippserver konnte nicht gestartet werden","GlobalData::init()");

    network::add_cert_path("/etc/ssl/certs");
    network::set_cert_file("/root/ssl/wetterfrosch.pem");

    sessions = std::vector<Session>(100);
    for (int i=0;i<sessions.size();i++) {
        sessions[i].log.prefix = std::string("session ")+std::to_string(i)+": ";
        sessions[i].log.parent = &log;
    }

    filters.init();
    tasks.init();
    actions.init();
}

void MainData::tick()
{
    int previous_work = workstate;
    workstate = WORK_STANDBY;
    for (Session& sess: sessions)
        sess.tick();
    updateTracker.tick();
    filters.tick();
    tasks.tick();
    actions.tick();
    if (workstate != previous_work)
        log << "Workload changed " << previous_work << " -> " << workstate << std::endl;
}

void MainData::request_work(int level)
{
    if (level<workstate)
        workstate = level;
}