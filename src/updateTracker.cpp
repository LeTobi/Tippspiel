#include "updateTracker.h"
#include "main-data.h"
#include <map>

using namespace tobilib;

const Time UpdateTracker::HISTORY_SIZE = 10*60;

const std::map<MsgType,std::string> tableNames {
    {MsgType::event,"Event"},
    {MsgType::eventTipp,"EventTipp"},
    {MsgType::game,"Game"},
    {MsgType::gameTipp,"GameTipp"},
    {MsgType::group,"Group"},
    {MsgType::player,"Player"},
    {MsgType::team,"Team"},
    {MsgType::user,"User"}
};

UpdateTracker::UpdateTracker()
{
    current = new HistoryEntry();
    log.prefix = "UpdateTracker: ";
}

UpdateTracker::~UpdateTracker()
{
    delete current;
    for (HistoryEntry* pt: history)
        delete pt;
}

void UpdateTracker::tick()
{
    Time now = get_time();

    // check for update release
    if (current->period_end <= now)
    {
        log << "Update gets distributed" << std::endl;

        // move history
        history.insert(history.begin(),current);
        current = new HistoryEntry();
        current->period_begin = history.front()->period_end;

        // distribute update
        h2rfp::JSObject msgdata = make_msg(history.front()->changes);
        for (Session& session: maindata->sessions)
        {
            if (session.status != Session::Status::active)
                continue;
            session.tasks.update.send_update(msgdata,now);
        }
    }

    // remove history overflow
    while (!history.empty() && history.back()->period_end < now-HISTORY_SIZE)
    {
        delete history.back();
        history.pop_back();
    }
}

void UpdateTracker::update(MsgID id, Time urgency)
{
    current->changes.insert(id);
    Time limit = get_time()+urgency;
    if (current->period_end > limit)
        current->period_end = limit;
}

Time UpdateTracker::horizon() const
{
    if (history.empty())
        return current->period_begin;
    return history.back()->period_begin;
}

h2rfp::JSObject UpdateTracker::get_history(Time begin) const
{
    std::set<MsgID> items;
    const auto it = history.begin();
    while (it!=history.end() && (**it).period_end>=begin)
        items.insert((**it).changes.begin(),(**it).changes.end());
    return make_msg(items);
}

h2rfp::JSObject UpdateTracker::make_msg(const std::set<MsgID>& change_items) const
{
    std::map<MsgType, h2rfp::JSObject> tables;
    for (const auto& change_item: change_items) {
        h2rfp::JSObject table_item;
        if (tableNames.count(change_item.type)!=0) {
            table_item.put_value(change_item.arg0);
            tables[change_item.type].push_back({"",table_item});
        }
    }

    h2rfp::JSObject output;
    for (auto& table: tables)
        output.put_child(tableNames.at(table.first),table.second);
    
    return output;
}