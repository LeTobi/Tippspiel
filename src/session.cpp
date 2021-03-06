#include "session.h"
#include "main-data.h"
#include "reqData/all.h"
#include "reqFunc/all.h"
#include "misc/response_util.h"
#include "sessionTasks/all.h"
#include "sessionTasks/pong.h"
#include "sessionTasks/cooldown.h"

using namespace tobilib;

Session::Session():
    client(*maindata->acceptor),
    tasks(*new SessionTasks(this))
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

Session::~Session()
{
    delete &tasks;
}

void Session::tick()
{
    client.tick();
    if (status != Status::inactive)
        tasks.tick();

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
    else if (tasks.pending()) {
        status = Status::cleanup;
    }
    else if (client.status()==h2rfp::EndpointStatus::closed) {
        log << "session beendet" << std::endl;
        user = Database::Cluster();
        client.connect();
        status = Status::inactive;
    }

    maindata->request_work(tasks.cooldown.session_usage());
}

void Session::on_connect()
{
    log << "client verbunden" << std::endl;
}

void Session::on_message(h2rfp::Message& msg)
{
    log << "Anfrage: " << msg.name << std::endl;

    if (msg.name == "signup")
        msg_handler::make_new_user(*this, msg);
    else if (msg.name == "restoreToken")
        msg_handler::restore_token(*this,msg);
    else if (msg.name == "signin")
        msg_handler::signin(*this, msg);
    else if (msg.name == "signout")
        msg_handler::signout(*this, msg);
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
    else if (msg.name == "nextPhase")
        msg_handler::game_shift_phase(*this,msg);
    else if (msg.name == "reportGame")
        msg_handler::game_report(*this,msg);
    else if (msg.name == "group_create")
        msg_handler::group_create(*this,msg);
    else if (msg.name == "group_rename")
        msg_handler::group_rename(*this,msg);
    else if (msg.name == "group_join")
        msg_handler::group_join(*this,msg);
    else if (msg.name == "group_leave")
        msg_handler::group_leave(*this,msg);
    else if (msg.name == "group_reset_token")
        msg_handler::group_reset_token(*this,msg);
    else if (msg.name == "ranking")
        msg_handler::get_event_ranking(*this,msg);
    else if (msg.name == "console")
        msg_handler::console_input(*this, msg);
    else {
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