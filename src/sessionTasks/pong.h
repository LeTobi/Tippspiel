#ifndef TIPPSPIEL_SESSIONTASKS_PONG_H
#define TIPPSPIEL_SESSIONTASKS_PONG_H

#include <tobilib/protocols/h2rfp.h>

class Session;

class SessionPong
{
public:
    Session* session;

    void tick();
    bool pending();

    void do_pong();

private:
    tobilib::h2rfp::Response response;
};

#endif