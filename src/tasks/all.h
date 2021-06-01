#ifndef TIPPSPIEL_TASKS_ALL_H
#define TIPPSPIEL_TASKS_ALL_H

class EmailTask;
class MsgDistributor;

class ServerTasks {
public:

    ServerTasks();
    ServerTasks(const ServerTasks&) = delete;
    ~ServerTasks();

    void tick();
    void init();

    EmailTask& emails;
    MsgDistributor& distribution;

};

#endif