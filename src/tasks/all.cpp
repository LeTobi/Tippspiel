#include "all.h"
#include "email.h"

ServerTasks::ServerTasks():
    emails(*new EmailTask())
{ }

ServerTasks::~ServerTasks()
{ 
    delete &emails;
}

void ServerTasks::tick()
{
    emails.tick();
}

void ServerTasks::init()
{
    emails.init();
}