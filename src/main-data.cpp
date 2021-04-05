#include "main-data.h"
#include <tobilib/ssl/network.h>

using namespace tobilib;

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

    network::ssl_client_ctx.add_verify_path("/etc/ssl/certs");
    network::ssl_server_init("/root/ssl/wetterfrosch.pem");

    sessions = std::vector<Session>(100);
    for (int i=0;i<sessions.size();i++) {
        sessions[i].log.prefix = std::string("session ")+std::to_string(i)+": ";
        sessions[i].log.parent = &log;
    }

    gametimeline.init();
}

MainData::~MainData()
{
    if (acceptor!=nullptr)
        delete acceptor;
}