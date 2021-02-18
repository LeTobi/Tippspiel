#ifndef TIPPSPIEL_EMAIL_H
#define TIPPSPIEL_EMAIL_H

#include "task.hpp"
#include "tobilib/stringplus/stringplus.h"
#include "tobilib/ssl/h2rfp.h"
#include "tobilib/general/queue.hpp"

class TokenEmail
{
public:
    struct Input {
        std::string name;
        std::string token;
        std::string email;
        unsigned int jsid;
        unsigned int userid;
    };

    struct Result {
        int result;
        unsigned int jsid;
    };

    TokenEmail();

    void tick();
    
    TaskManager<Input, Result> tasks;

private:
    tobilib::Logger log;
    tobilib::network::SSL_Endpoint socket;
    tobilib::StringPlus received;
    unsigned int tries = 0;
    Result result;

    void begin_cycle();
    void send();
    void checkHttpResponse();
    void end_cycle_success();
    void end_cycle_error();
};

#endif