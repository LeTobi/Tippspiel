#ifndef TIPPSPIEL_TASK_REGISTRATION
#define TIPPSPIEL_TASK_REGISTRATION

#include "../misc/task.hpp"
#include <tobilib/database/database.h>
#include <tobilib/ssl/h2rfp.h>
#include <tobilib/general/timer.hpp>

class EmailTask
{
public:
    enum class Type {
        registration,
        restore_token
    };

    struct Input {
        Type type;
        std::string new_token;
        std::string lang;
        tobilib::Database::Cluster user;
    };

    struct Result {
        bool success;
        std::string msg;
    };

    void init();
    void tick();
    
    TaskManager<Input, Result> tasks;

private:
    tobilib::Logger log;
    tobilib::network::SSL_Endpoint* socket = nullptr;
    tobilib::StringPlus received;
    tobilib::Timer retry_timeout;
    unsigned int tries = 0;
    Result result;

    void send_http_request(const std::string&, const tobilib::h2rfp::JSObject&);
    void send_register_mail();
    void send_restore_mail();

    void on_new_task();
    void on_connect();
    void on_response();
    void on_response_success();
    void on_response_error();
    void on_config_fail(const char*);
};

#endif