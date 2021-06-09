#ifndef TIPPSPIEL_TASKS_MSGDISTRIBUTOR_H
#define TIPPSPIEL_TASKS_MSGDISTRIBUTOR_H

#include <tobilib/protocols/h2rfp.h>

class MsgDistributor
{
public:

    void simple_no_response_to_all(const std::string&) const;
    void data_no_response_to_all(const std::string& name, const tobilib::h2rfp::JSObject& data) const;

private:

};

#endif