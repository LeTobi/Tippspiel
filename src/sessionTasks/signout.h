#ifndef TIPPSPIEL_SESSIONTASKS_SIGNOUT_H
#define TIPPSPIEL_SESSIONTASKS_SIGNOUT_H

class Session;

class SessionSignout
{
public:
    Session* session;

    void tick();
    bool pending() const;

    void signout(unsigned int rid);

private:
    bool signout_in_process = false;
    unsigned int msg_id;
};

#endif