#ifndef TIPPSPIEL_SESSION_H
#define TIPPSPIEL_SESSION_H

#include <tobilib/ssl/h2rfp.h>
#include <tobilib/database/database.h>

class SessionTasks;

class Session {
public:
    enum class Status {
        inactive,
        active,
        cleanup
    };

    Status status = Status::inactive;
    tobilib::h2rfp::WSS_Endpoint client;
    tobilib::Logger log;
    tobilib::Database::Cluster user;
    SessionTasks& tasks;

    Session();
    Session(const Session&) = delete;
    ~Session();
    void tick();

private:
    void on_connect();
    void on_message(tobilib::h2rfp::Message&);
    void on_inactive();
    void on_disconnect();
};

#endif