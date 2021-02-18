#include "./global.h"
#include "eventhandler.h"
#include <thread>

using namespace tobilib;
using namespace network;

bool init()
{
    ssl_client_ctx.add_verify_path("/etc/ssl/certs");
    ssl_server_init("/root/ssl/wetterfrosch.pem");
    data = new GlobalData();

    data->database.init();
    data->database.open();

    if (!data->database.is_good())
        return false;

    return true;
}

void client_connect(Client& client)
{

}

void client_message(Client& client, h2rfp::Message msg)
{
    if (msg.name == "signup")
        signup(client, msg);
    else if (msg.name == "signin")
        signin(client, msg);
    else if (msg.name == "me")
        inform_identity(client, msg);
    else if (msg.name == "get_data")
        serve_data(client, msg);
    else if (msg.name == "console")
        console_input(client, msg);
    else
        data->log << "unknown message: " << msg.name << std::endl;
}

void client_inactive(Client& client)
{

}

void client_close(Client& client)
{
    client.user = Database::Cluster();
    client.emailtask = 0;
}

void client_tick(Client& client)
{
    client.endpoint.tick();
    while (!client.endpoint.events.empty()) {
        h2rfp::EndpointEvent ev = std::move(client.endpoint.events.next());
        if (ev.type == h2rfp::EventType::connected)
            client_connect(client);
        if (ev.type == h2rfp::EventType::message)
            client_message(client, ev.msg);
        if (ev.type == h2rfp::EventType::inactive)
            client_inactive(client);
        if (ev.type == h2rfp::EventType::closed)
            client_close(client);
    }

    if (client.emailtask!=0 && data->emails.tasks.isFinished(client.emailtask))
        signup_complete(client, data->emails.tasks.getResult(client.emailtask));
}

int main()
{
    if (!init())
        return 0;
    std::cout << "init complete" << std::endl;

    while (true)
    {
        std::this_thread::yield();
        for (Client& cli: data->clients)
            client_tick(cli);
        data->emails.tick();

        if (!data->database.is_good())
        {
            data->log << "Programm beendet wegen fehler in der Datenbank" << std::endl;
            break;
        }
    }

    return 0;
}