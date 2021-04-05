#ifndef TIPPSPIEL_TASK_REGISTRATION
#define TIPPSPIEL_TASK_REGISTRATION

#include "../misc/task.hpp"
namespace tasks {

    class Registration
    {
    public:
        struct Input {
            std::string name;
            std::string token;
            std::string email;
            unsigned int jsid;
            unsigned int userid;
        };

        struct Result {
            int result;
            unsigned int jsid;
        };

        TaskManager<Input,Result> tasks;

    };

}

#endif