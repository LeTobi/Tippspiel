#include "all.h"
#include "email.h"
#include "msgDistributor.h"
#include "push.h"

ServerTasks::ServerTasks():
    emails(*new EmailTask()),
    distribution(*new MsgDistributor()),
    push(*new PushTask())
{ }

ServerTasks::~ServerTasks()
{ 
    delete &emails;
    delete &distribution;
    delete &push;
}

void ServerTasks::tick()
{
    emails.tick();
    push.tick();
}

void ServerTasks::init()
{
    emails.init();
    push.init();
}