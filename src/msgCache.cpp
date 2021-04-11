#include "msgCache.h"
#include "msgCreation/all.h"
#include "session.h"

using namespace tobilib;
using namespace h2rfp;

const int MsgCache::CACHE_SIZE = 100;

MsgCache::Entry::Entry(const MsgID& _id): id(_id)
{ }

JSObject MsgCache::get_data(Session& session, const MsgID& id)
{
    if (msg_creation::is_individual(session,id))
        return msg_creation::make_msg(session,id);
    stat_requests++;
    for (int i=0;i<entries.size();i++)
    {
        if (entries[i].id != id)
            continue;
        stat_hits++;
        if (i==0)
            return entries[0].data;
        std::swap(entries[i],entries[i-1]);
        return entries[i-1].data;
    }
    if (entries.size() >= CACHE_SIZE)
        entries.pop_back();
    unsigned int offset = entries.size()/2;
    entries.insert(entries.begin()+offset,MsgID(id));
    entries[offset].data = msg_creation::make_msg(session,id);
    return entries[offset].data;
}

int MsgCache::find_entry(const MsgID& id) const
{
    for (int i=0;i<entries.size();i++)
    {
        if (entries[i].id == id)
            return i;
    }
    return -1;
}

int MsgCache::size() const
{
    return entries.size();
}

void MsgCache::clear()
{
    stat_hits = 0;
    stat_requests = 0;
    entries.clear();
}

void MsgCache::remove(const MsgID& id)
{
    for (auto it=entries.begin();it!=entries.end();++it)
        if (it->id == id) {
            entries.erase(it);
            break;
        }
}