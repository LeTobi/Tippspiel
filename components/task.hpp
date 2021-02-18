#ifndef TIPPSPIEL_TASK_H
#define TIPPSPIEL_TASK_H

#include <vector>
#include "tobilib/general/exception.hpp"
#include "tobilib/general/queue.hpp"

using Task = unsigned int;

template<class T_Input, class T_Result>
class TaskManager {
public:
    Task makeTask(T_Input& in) {
        nextid = nextid%10000 + 1;
        tasks.push(std::make_pair(nextid, in));
        return nextid;
    }

    bool isFinished(Task t) const {
        for (auto& item: results)
            if (item.first==t)
                return true;
        return false;
    }

    T_Result getResult(Task t) {
        for (auto it = results.begin(); it!=results.end(); it++)
        {
            if (it->first == t)
            {
                T_Result out = it->second;
                results.erase(it);
                return out;
            }
        }
        throw tobilib::Exception("Der Angeforderte Task ist nicht beendet.","TaskManager::getResult()");
    }

    bool hasTask() const {
        return !tasks.empty();
    }

    T_Input& getCurrent() {
        return tasks.front().second;
    }

    void finishCurrent(T_Result res) {
        results.push_back(std::make_pair(tasks.front().first, res));
        tasks.pop();
        if (results.size()>20)
            results.erase(results.begin());
    }

private:
    unsigned int nextid = 0;
    tobilib::Queue<std::pair<Task, T_Input>> tasks;
    std::vector<std::pair<Task, T_Result>> results;
};

#endif