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
            return_result(*session,response_id,make_result());
        }
        else
        {
            return_client_error(*session,response_id,res.msg);
        }
        task=NO_TASK;
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

    Database::Cluster user = data_edit::create_user(name,email);
    response_id = rid;
    new_token = data_edit::make_new_token(user);
    data_edit::set_user_token(user,new_token);

    EmailTask::Input params;
    params.new_token = new_token;
    params.user = user;
    params.type = EmailTask::Type::registration;
    task = maindata->tasks.emails.tasks.makeTask(params);
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