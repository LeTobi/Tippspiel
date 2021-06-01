#include "all.h"
#include "email.h"
#include "msgDistributor.h"

ServerTasks::ServerTasks():
    emails(*new EmailTask()),
    distribution(*new MsgDistributor())
{ }

ServerTasks::~ServerTasks()
{ 
    delete &emails;
    delete &distribution;
}

void ServerTasks::tick()
{
    emails.tick();
}

void ServerTasks::init()
{
    emails.init();
}