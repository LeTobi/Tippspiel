#ifndef TIPPSPIEL_SESSIONTASK_TOKEN_H
#define TIPPSPIEL_SESSIONTASK_TOKEN_H

#include <string>
#include <tobilib/database/database.h>
#include "../misc/task.hpp"
class Session;

class RegisterTask
{
public:
    Session* session;

    void tick();
    bool pending();

    void make_new_user(const std::string& name, const std::string& email, unsigned int rid);

private:
    unsigned int response_id;
    tobilib::Database::Cluster new_user;
    Task task = NO_TASK;
};

class RestoreTask
{
public:
    Session* session;

    void tick();
    bool pending();

    void restore_token(tobilib::Database::Cluster user, unsigned int rid);

private:
    unsigned int response_id;
    tobilib::Database::Cluster target_user;
    std::string new_token;
    Task task = NO_TASK;
};

#endif