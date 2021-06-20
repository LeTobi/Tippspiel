#ifndef TIPPSPIEL_TASKS_PUSH
#define TIPPSPIEL_TASKS_PUSH

#include <set>
#include <tobilib/database/database.h>
#include <tobilib/protocols/h2rfp.h>

class PushTask
{
public:
    void init();
    void tick();

    void game_reminder(tobilib::Database::Cluster game);
    void game_results(tobilib::Database::Cluster game);
    void event_reminder(tobilib::Database::Cluster event);
    void event_results(tobilib::Database::Cluster event);

private:
    tobilib::Logger log = std::string("PushTask: ");

    int send_process = 0;
    std::vector<tobilib::h2rfp::JSObject> out_data;

    void enqueue(tobilib::Database::Cluster sub, const std::string& type, tobilib::h2rfp::JSObject data);
    void send();
    void get_send_result(int status);
};

#endif