#ifndef TIPPSPIEL_SESSIONTASKS_H
#define TIPPSPIEL_SESSIONTASKS_H

class Session;
class SessionUpdate;
class SessionPong;
class RegisterTask;
class RestoreTask;

class SessionTasks {
public:

    SessionTasks(Session*);
    SessionTasks(const SessionTasks&) = delete;
    ~SessionTasks();

    void tick();
    bool pending();

    SessionUpdate& update;
    SessionPong& pong;
    RegisterTask& registration;
    RestoreTask& token_restore;

};

#endif