#include "./global.h"
#include "session/eventhandler.h"
#include "session/utils.h"
#include "session/filter.h"
#include "manipulation/game.h"
#include "manipulation/group.h"
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

    data->observer.setup();

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
    else if (msg.name == "hotGames")
        filter_hot_games(client, msg);
    else if (msg.name == "suggest_locations")
        suggest_locations(client, msg);
    else if (msg.name == "suggest_players")
        suggest_players(client,msg);
    else if (msg.name == "createPlayer")
        create_player(client,msg);
    else if (msg.name == "makeTipp")
        make_tipp(client,msg);
    else if (msg.name == "createGame")
        create_game(client,msg);
    else if (msg.name == "reportGame")
        report_game(client,msg);
    else if (msg.name == "console")
        console_input(client, msg);
    else
        return_client_error(client,msg,"Diese Funktion ist nicht teil der API");
}

void client_inactive(Client& client)
{
    if (client.pong.is_requested()) {
        client.endpoint.close();
    } else {
        client.pong = client.endpoint.request("Ping");
    }
}

void client_close(Client& client)
{
    client.user = Database::Cluster();
    client.emailtask = 0;
    client.updates.clear();
    client.endpoint.responses.clear();
    client.endpoint.connect();
}

void client_tick(Client& client)
{
    client.endpoint.tick();

    if (client.emailtask!=0 && data->emails.tasks.isFinished(client.emailtask))
        signup_complete(client, data->emails.tasks.getResult(client.emailtask));

    auto it = client.updates.rbegin();
    while (it!=client.updates.rend()) {
        it->response.update(client.endpoint.responses);
        if (it->response.is_received()) {
            client.user["lastupdate"].set( it->time );
            ++it;
            client.updates.erase(it.base()); // reverse iterator hat offset 1
        } else {
            ++it;
        }
    }

    client.pong.update(client.endpoint.responses);
    if (client.pong.is_received());
        client.pong.dismiss();

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
}

void distribute_update(const VirtualDB::Update& update)
{
    for (Client& client: data->clients)
    {
        if (client.endpoint.status() != h2rfp::EndpointStatus::connected || client.user.is_null())
            continue;
        client.updates.emplace_back();
        client.updates.back().response = client.endpoint.request("Update",update.data);
        client.updates.back().time = update.time;
    }
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
        data->virtualdb.tick();
        data->observer.tick();
        while (!data->virtualdb.updates.empty())
            distribute_update(data->virtualdb.updates.next());

        if (!data->database.is_good())
        {
            data->log << "Programm beendet wegen fehler in der Datenbank" << std::endl;
            break;
        }
    }

    return 0;
}