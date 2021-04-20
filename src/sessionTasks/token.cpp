#include "token.h"
#include "../session.h"
#include "../main-data.h"
#include "../misc/response_util.h"
#include "../dataEdit/user.h"
#include "../dataEdit/token.h"
#include "../tasks/all.h"
#include "../tasks/email.h"

using namespace tobilib;

void RegisterTask::tick()
{
    if (maindata->tasks.emails.tasks.isFinished(task))
    {
        EmailTask::Result res = maindata->tasks.emails.tasks.getResult(task);
        if (res.success)
        {
            session->log << new_user["name"].get<std::string>() << " (id " << new_user.index() << ") nimmt jetzt am tippspiel teil" << std::endl;
            return_result(*session,response_id,make_result());
        }
        else
        {
            session->log << "Beitritt fehlgeschlagen, Nutzer wird entfernt" << std::endl;
            data_edit::delete_user(new_user);
            return_client_error(*session,response_id,res.msg);
        }
        task=NO_TASK;
        new_user = Database::Cluster();
    }
}

bool RegisterTask::pending()
{
    return task != NO_TASK;
}

void RegisterTask::make_new_user(const std::string& name, const std::string& email, unsigned int rid)
{
    if (pending())
    {
        return_result(*session,rid,make_user_error(ERROR_ALREADY_RUNNING));
        return;
    }

    new_user = data_edit::create_user(name,email);
    response_id = rid;
    std::string new_token = data_edit::make_new_token(new_user);
    data_edit::set_user_token(new_user,new_token);

    EmailTask::Input params;
    params.new_token = new_token;
    params.user = new_user;
    params.type = EmailTask::Type::registration;
    task = maindata->tasks.emails.tasks.makeTask(params);

    session->log << "create_user request: id " << new_user.index() << ", " << name << std::endl;
}

void RestoreTask::tick()
{
    if (maindata->tasks.emails.tasks.isFinished(task))
    {
        EmailTask::Result res = maindata->tasks.emails.tasks.getResult(task);
        if (res.success)
        {
            data_edit::set_user_token(target_user,new_token);
            return_result(*session,response_id,make_result());
        }
        else
        {
            return_client_error(*session,response_id,res.msg);
        }
        task=NO_TASK;
    }
}

bool RestoreTask::pending()
{
    return task != NO_TASK;
}

void RestoreTask::restore_token(tobilib::Database::Cluster user, unsigned int rid)
{
    if (pending())
    {
        return_result(*session,rid,make_user_error(ERROR_ALREADY_RUNNING));
        return;
    }

    target_user = user;
    response_id = rid;
    new_token = data_edit::make_new_token(target_user);

    EmailTask::Input params;
    params.new_token = new_token;
    params.user = target_user;
    params.type = EmailTask::Type::restore_token;
    task = maindata->tasks.emails.tasks.makeTask(params);
}