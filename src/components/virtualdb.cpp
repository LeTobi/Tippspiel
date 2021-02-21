#include "virtualdb.h"
#include "../global.h"
#include <tobilib/general/exception.hpp>
#include <utility>
#include <cmath>

const Time VirtualDB::WAIT_ENDLESS = 3600;
const Time VirtualDB::WAIT_LONG = 60;
const Time VirtualDB::WAIT_SHORT = 10;
const Time VirtualDB::WAIT_NOT = 0;

const int VirtualDB::CACHE_SIZE = 500;
const Time VirtualDB::HISTORY_SIZE = 10*60;

std::vector<std::string> VirtualDB::TableNames = {
    "User", "Group", "Player", "Team", "Event", "Game", "EventTipp", "GameTipp"
};

using namespace tobilib;

VirtualDB::Entry::Entry(int t, unsigned int i):
    table(t),
    idx(i)
{ }

VirtualDB::Entry::Entry(const Database::Cluster& cluster)
{
    if (cluster.is_null())
    {
        table = TABLE_USER;
        idx = 0;
        return;
    }
    table = get_table(cluster.type().name);
    if (table<0)
        throw tobilib::Exception("Cluster is not part of sync","VirtualDB::Entry constructor");
    idx = cluster.index();
}

VirtualDB::Entry::operator Database::Cluster() const
{
    return ::data->database.list(TableNames[table])[idx];
}

bool VirtualDB::Entry::operator==(const Entry& other) const
{
    return other.table == table && other.idx == idx;
}

bool VirtualDB::Entry::operator!=(const Entry& other) const
{
    return !(*this==other);
}

VirtualDB::CacheEntry::CacheEntry(const Entry& ent):
    dbentry(ent)
{ }

bool VirtualDB::Entry::operator<(const Entry& other) const
{
    return (table==other.table&&idx<other.idx) || table<other.table;
}

VirtualDB::VirtualDB()
{
    cache.reserve(CACHE_SIZE);
}

int VirtualDB::get_table(const std::string& name)
{
    for (int i=0;i<TableNames.size();i++)
        if (TableNames[i]==name)
            return i;
    return -1;
}

h2rfp::JSObject VirtualDB::get_data(Entry wanted)
{
    stat_requests++;
    for (int i=0;i<cache.size();i++)
    {
        if (cache[i].dbentry != wanted)
            continue;
        stat_caches++;
        if (i==0)
            return cache[0].msg;
        std::swap(cache[i],cache[i-1]);
        return cache[i-1].msg;
        
    }
    if (cache.size() >= CACHE_SIZE)
        cache.pop_back();
    cache.insert(cache.begin(),CacheEntry(wanted));
    cache.front().msg = data(wanted);
    return cache.front().msg;
}

int VirtualDB::find_entry(Entry wanted)
{
    for (int i=0;i<cache.size();i++)
    {
        if (cache[i].dbentry==wanted)
            return i;
    }
    return -1;
}

h2rfp::JSObject VirtualDB::get_history(Time from)
{
    std::set<Entry> items;
    for (int i=history.size()-1; i>=1; i--)
        if (history[i].period_end >= from)
            items.insert(history[i].entries.begin(),history[i].entries.end());
    return set_to_update(items);
}

Time VirtualDB::get_history_support()
{
    if (history.empty())
        return get_time();
    return history.back().period_begin;
}

void VirtualDB::clear_cache()
{
    cache.clear();
    stat_requests = 0;
    stat_caches = 0;
}

void VirtualDB::update(Entry entry, Time max_wait)
{
    if (entry.table == TABLE_GAME)
        ::data->observer.update(entry);

    for (auto it=cache.begin();it!=cache.end();++it)
        if (it->dbentry == entry)
            cache.erase(it);
    HistoryEntry& now = history_now();
    now.entries.insert(entry);
    now.period_end = std::min(get_time()+max_wait, now.period_end);
}

void VirtualDB::tick()
{
    if (history_now().period_end <= get_time())
        history_advance();
}

h2rfp::JSObject VirtualDB::set_to_update(const std::set<Entry>& entries)
{
    std::vector<h2rfp::JSObject> tablelists (TableNames.size());
    for (const Entry& entry: entries) {
        h2rfp::JSObject item;
        item.put_value(entry.idx);
        tablelists[entry.table].push_back(std::make_pair("",item));
    }
    h2rfp::JSObject output;
    for (int i=0;i<tablelists.size();i++)
        if (!tablelists[i].empty())
            output.put_child(TableNames[i],tablelists[i]);
    return output;
}

VirtualDB::HistoryEntry& VirtualDB::history_now()
{
    if (!history.empty())
        return history.front();
    history_advance();
    return history.front();
}

void VirtualDB::history_advance()
{
    Time now = get_time();

    if (!history.empty())
    {
        Update update;
        update.data = set_to_update(history.front().entries);
        update.time = now;
        updates.push(std::move(update));
    }

    history.emplace(history.begin());
    history.front().period_begin = now;
    history.front().period_end = now + WAIT_ENDLESS;

    while (history.back().period_begin < now-HISTORY_SIZE)
        history.pop_back();
}

h2rfp::JSObject VirtualDB::data(Entry entry)
{
    switch (entry.table)
    {
        case TABLE_USER:
            return data_user(entry);
        case TABLE_GROUP:
            return data_group(entry);
        case TABLE_PLAYER:
            return data_player(entry);
        case TABLE_TEAM:
            return data_team(entry);
        case TABLE_EVENT:
            return data_event(entry);
        case TABLE_GAME:
            return data_game(entry);
        case TABLE_ETIPP:
            return data_eventTipp(entry);
        case TABLE_GTIPP:
            return data_gameTipp(entry);
        default:
            throw Exception("Tabelle existiert nicht","VirtualDB::data()");
    }
}

h2rfp::JSObject VirtualDB::data_user(Database::Cluster user)
{
    h2rfp::JSObject out;
    if (user.is_null())
        return out;
    out.put("id",user.index());
    out.put("name",user["name"].get<std::string>());
    out.put("points",user["points"].get<int>());
    return out;
}

h2rfp::JSObject VirtualDB::data_group(Database::Cluster group)
{
    h2rfp::JSObject out;
    if (group.is_null())
        return out;
    out.put("id",group.index());
    out.put("name",group["name"].get<std::string>());
    h2rfp::JSObject members;
    for (auto member: group["users"]) {
        h2rfp::JSObject item;
        item.put_value(member->index());
        members.push_back(std::make_pair("",item));
    }
    out.put_child("users",members);
    return out;
}

h2rfp::JSObject VirtualDB::data_player(Database::Cluster player)
{
    h2rfp::JSObject out;
    if (player.is_null())
        return out;
    out.put("id",player.index());
    out.put("name",player["name"].get<std::string>());
    out.put("team",player["team"]->index());
    return out;
}

h2rfp::JSObject VirtualDB::data_team(Database::Cluster team)
{
    h2rfp::JSObject out;
    if (team.is_null())
        return out;
    out.put("id",team.index());
    out.put("name",team["name"].get<std::string>());
    out.put("short", team["short"].get<std::string>());
    h2rfp::JSObject list;
    for (auto game: team["games"])
    {
        h2rfp::JSObject item;
        item.put_value(game->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("games",list);
    list.clear();
    for (auto player: team["players"])
    {
        h2rfp::JSObject item;
        item.put_value(player->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("players",list);
    return out;
}

h2rfp::JSObject VirtualDB::data_event(Database::Cluster event)
{
    h2rfp::JSObject out;
    if (event.is_null())
        return out;
    out.put("id",event.index());
    out.put("name",event["name"].get<std::string>());
    out.put("description", event["description"].get<std::string>());
    out.put("deadline", event["deadline"].get<int>());
    out.put("status",event["eventStatus"].get<int>());
    h2rfp::JSObject list;
    for (auto game: event["games"])
    {
        h2rfp::JSObject item;
        item.put_value(game->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("games",list);
    list.clear();
    for (auto tipp: event["tipps"])
    {
        h2rfp::JSObject item;
        item.put_value(tipp->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("tipps",list);
    out.put("winner",event["winner"]->index());
    out.put("topscorer",event["topscorer"]->index());
    return out;
}

h2rfp::JSObject VirtualDB::data_game(Database::Cluster game)
{
    h2rfp::JSObject out;
    if (game.is_null())
        return out;
    out.put("id",game.index());
    out.put("start",game["start"].get<int>());
    out.put("location",game["location"]["name"].get<std::string>());
    out.put("stream",game["stream"].get<std::string>());
    out.put("event",game["event"]->index());
    out.put("status",game["gameStatus"].get<int>());
    out.put("phase",game["phase"].get<int>());
    out.put("team1",game["teams"][0]->index());
    out.put("team2",game["teams"][1]->index());
    out.put("score1",game["scores"][0].get<int>());
    out.put("score2",game["scores"][1].get<int>());
    out.put("penaltyScore1",game["penalty"][0].get<int>());
    out.put("penaltyScore2",game["penalty"][1].get<int>());
    h2rfp::JSObject list;
    for (auto scorer: game["scorers"])
    {
        h2rfp::JSObject item;
        item.put_value(scorer->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("scorers",list);
    list.clear();
    for (auto tipp: game["tipps"])
    {
        h2rfp::JSObject item;
        item.put_value(tipp->index());
        list.push_back(std::make_pair("",item));
    }
    out.put_child("tipps",list);

    // not synced:
    out.put("short1", game["teams"][0]["short"].get<std::string>());
    out.put("short2", game["teams"][1]["short"].get<std::string>());
    return out;
}

h2rfp::JSObject VirtualDB::data_eventTipp(Database::Cluster etipp)
{
    h2rfp::JSObject out;
    if (etipp.is_null())
        return out;
    out.put("id",etipp.index());
    out.put("event",etipp["event"]->index());
    out.put("user",etipp["user"]->index());
    out.put("reward",etipp["reward"].get<int>());

    if (etipp["event"]["deadline"].get<int>() > get_time())
        return out;

    out.put("winner",etipp["winner"]->index());
    out.put("topscorer",etipp["topscorer"]->index());
    return out;
}

h2rfp::JSObject VirtualDB::data_gameTipp(Database::Cluster gtipp)
{
    h2rfp::JSObject out;
    if (gtipp.is_null())
        return out;
    out.put("id",gtipp.index());
    out.put("game",gtipp["game"]->index());
    out.put("user",gtipp["user"]->index());
    out.put("reward",gtipp["reward"].get<int>());

    if (gtipp["game"]["start"].get<int>() > get_time())
        return out;

    out.put("bet1",gtipp["bet"][0].get<int>());
    out.put("bet2",gtipp["bet"][1].get<int>());
    out.put("winner",gtipp["winner"]->index());
    out.put("topscorer",gtipp["topscorer"]->index());
    return out;
}