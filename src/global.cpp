#include "global.h"

using namespace tobilib;

GlobalData* data;
TokenEmail* emails;

network::Acceptor* Client::acceptor = nullptr;

Client::Client(): endpoint(*acceptor)
{
    endpoint.log.parent = &data->log;
}

GlobalData::GlobalData()
{ }

void GlobalData::init() {

    logfile.open("logfile",logfile.out | logfile.app);
    if (logfile.good()) {
        log.print_time = true;
        log.dump = &logfile;
    } else {
        log.dump = &std::cerr;
        throw Exception("Kein zugriff auf die Logdatei","GlobalData::init()");
    }

    if (Client::acceptor != nullptr)
        throw Exception("Mehrfache initialisierung, Programmierfehler","GlobalData::init()");
    Client::acceptor = new network::Acceptor (15320);


    database.log.parent = &log;
    database.setPath("../../data/");
    database.init();
    database.open();

    if (!database.is_good())
        throw Exception("Tippserver konnte nicht gestartet werden","GlobalData::init()");

    network::ssl_client_ctx.add_verify_path("/etc/ssl/certs");
    network::ssl_server_init("/root/ssl/wetterfrosch.pem");

    clients = std::vector<Client>(100);
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

    observer.setup();
}

GlobalData::~GlobalData()
{
    if (Client::acceptor!=nullptr)
        delete Client::acceptor;
}