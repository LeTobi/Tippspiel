#include "session.h"
#include "main-data.h"
#include "reqData/all.h"
#include "reqFunc/all.h"
#include "misc/response_util.h"

using namespace tobilib;

Session::Session(): client(*maindata->acceptor), tasks(this)
{ 
    client.log.parent = &log;
    client.options.handshake_timeout = 5;
    client.options.read_timeout      = 10;
    client.options.inactive_warning  = 5;
    client.options.send_timeout      = 0;
    client.options.connect_timeout   = 0;
    client.options.close_timeout     = 2;
    client.connect();
}

void Session::tick()
{
    client.tick();
    if (status != Status::inactive)
        tasks.tick_all();

    while (!client.events.empty()) {
        h2rfp::EndpointEvent ev = std::move(client.events.next());
        if (ev.type == h2rfp::EventType::connected)
            on_connect();
        if (ev.type == h2rfp::EventType::message)
            on_message(ev.msg);
        if (ev.type == h2rfp::EventType::inactive)
            on_inactive();
        if (ev.type == h2rfp::EventType::closed)
            on_disconnect();
    }

    if (client.status() == h2rfp::EndpointStatus::connected) {
        status = Status::active;
    }
    else if (tasks.any_pending()) {
        status = Status::cleanup;
    }
    else if (client.status()==h2rfp::EndpointStatus::closed) {
        log << "session beendet" << std::endl;
        user = Database::Cluster();
        client.connect();
        status = Status::inactive;
    }
}

void Session::on_connect()
{
    log << "client verbunden" << std::endl;
}

void Session::on_message(h2rfp::Message& msg)
{
    log << "Anfrage: " << msg.name << std::endl;

    /*if (msg.name == "signup")
        signup(client, msg);
    else*/ if (msg.name == "signin")
        msg_handler::signin(*this, msg);
    else if (msg.name == "me")
        msg_handler::inform_identity(*this, msg);
    else if (msg.name == "get_data")
        msg_handler::sync(*this, msg);
    else if (msg.name == "hotGames")
        msg_handler::hot_games(*this, msg);
    else if (msg.name == "suggest_locations")
        msg_handler::suggest_locations(*this, msg);
    else if (msg.name == "suggest_players")
        msg_handler::suggest_players(*this,msg); // brauchts das?
    else if (msg.name == "createPlayer")
        msg_handler::create_player(*this,msg);
    else if (msg.name == "makeTipp")
        msg_handler::game_tipp(*this,msg);
    else if (msg.name == "createGame")
        msg_handler::game_announce(*this,msg);
    /*else if (msg.name == "reportGame")
        report_game(client,msg);
    else if (msg.name == "console")
        console_input(client, msg);*/
    else {
        log << "Diese Anfrage ist nicht teil der API" << std::endl;
        return_client_error(*this,msg,"Diese Funktion ist nicht teil der API");
    }
}

void Session::on_inactive()
{
    tasks.pong.do_pong();
}

void Session::on_disconnect()
{
    log << "client getrennt" << std::endl;
}