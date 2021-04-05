#ifndef TIPPSPIEL_SESSIONTASK_UPDATE_H
#define TIPPSPIEL_SESSIONTASK_UPDATE_H

#include "../misc/time.h"
#include <tobilib/protocols/h2rfp.h>
#include <vector>

class Session;

class SessionUpdate
{
public:
    Session* session;

    void tick();
    bool pending();

    void send_update(const tobilib::h2rfp::JSObject&, Time);

private:
    struct Request {
        Time synctime;
        tobilib::h2rfp::Response response;
    };

    std::vector<Request> requests;
};

#endif