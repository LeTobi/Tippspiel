#include "virtualdb.h"
#include "../global.h"
#include <tobilib/general/exception.hpp>
#include <utility>

const Time VirtualDB::WAIT_ENDLESS = 3600;
const Time VirtualDB::WAIT_LONG = 60;
const Time VirtualDB::WAIT_SHORT = 10;
const Time VirtualDB::WAIT_NOT = 0;

const int VirtualDB::CACHE_SIZE = 500;
const Time VirtualDB::HISTORY_SIZE = 10*60;

std::vector<std::string> VirtualDB::TableNames = {
    "users", "groups", "players", "teams", "events", "games", "etipps", "gtipps"
};

using namespace tobilib;

VirtualDB::Entry::Entry(int t, unsigned int i):
    table(t),
    idx(i)
{ }

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

h2rfp::JSObject VirtualDB::get_entry(int t,unsigned int i)
{
    Entry wanted(t,i);
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

int VirtualDB::find_entry(int t, unsigned int i)
{
    Entry wanted(t,i);
    for (int i=0;i<cache.size();i++)
    {
        if (cache[i].dbentry==wanted)
            return i;
    }
    return -1;
}

h2rfp::JSObject VirtualDB::get_history(Time)
{

}

Time VirtualDB::get_history_support()
{

}

void VirtualDB::clear_cache()
{
    cache.clear();
    stat_requests = 0;
    stat_caches = 0;
}

h2rfp::JSObject VirtualDB::data(Entry entry)
{
    switch (entry.table)
    {
        case Tables::users:
            return data_user(entry.idx);
        case Tables::groups:
            return data_group(entry.idx);
        case Tables::players:
            return data_player(entry.idx);
        case Tables::teams:
            return data_team(entry.idx);
        case Tables::events:
            return data_event(entry.idx);
        case Tables::games:
            return data_game(entry.idx);
        case Tables::etipps:
            return data_eventTipp(entry.idx);
        case Tables::gtipps:
            return data_gameTipp(entry.idx);
        default:
            throw Exception("Tabelle existiert nicht","VirtualDB::data()");
    }
}

h2rfp::JSObject VirtualDB::data_user(unsigned int idx)
{
    h2rfp::JSObject out;
    Database::Cluster user = ::data->database.list("User")[idx];
    if (user.is_null())
        return out;
    out.put("id",user.index());
    out.put("name",user["name"].get<std::string>());
    out.put("points",user["points"].get<int>());
    return out;
}

h2rfp::JSObject VirtualDB::data_group(unsigned int idx)
{
    h2rfp::JSObject out;
    Database::Cluster group = ::data->database.list("Group")[idx];
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

h2rfp::JSObject VirtualDB::data_player(unsigned int idx)
{
    h2rfp::JSObject out;
    Database::Cluster player = ::data->database.list("Player")[idx];
    if (player.is_null())
        return out;
    out.put("id",player.index());
    out.put("name",player["name"].get<std::string>());
    out.put("team",player["team"]->index());
    return out;
}

h2rfp::JSObject VirtualDB::data_team(unsigned int idx)
{
    h2rfp::JSObject out;
    Database::Cluster team = ::data->database.list("Team")[idx];
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

h2rfp::JSObject VirtualDB::data_event(unsigned int idx)
{
    h2rfp::JSObject out;
    Database::Cluster event = ::data->database.list("Event")[idx];
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

h2rfp::JSObject VirtualDB::data_game(unsigned int idx)
{
    h2rfp::JSObject out;
    Database::Cluster game = ::data->database.list("Game")[idx];
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

h2rfp::JSObject VirtualDB::data_eventTipp(unsigned int idx)
{
    h2rfp::JSObject out;
    Database::Cluster etipp = ::data->database.list("EventTipp")[idx];
    if (etipp.is_null())
        return out;
    out.put("id",etipp.index());
    out.put("event",etipp["event"]->index());
    out.put("user",etipp["user"]->index());
    out.put("winner",etipp["winner"]->index());
    out.put("topscorer",etipp["topscorer"]->index());
    out.put("reward",etipp["reward"].get<int>());
    return out;
}

h2rfp::JSObject VirtualDB::data_gameTipp(unsigned int idx)
{
    h2rfp::JSObject out;
    Database::Cluster gtipp = ::data->database.list("GameTipp")[idx];
    if (gtipp.is_null())
        return out;
    out.put("id",gtipp.index());
    out.put("game",gtipp["game"]->index());
    out.put("user",gtipp["user"]->index());
    out.put("bet1",gtipp["bet"][0].get<int>());
    out.put("bet2",gtipp["bet"][1].get<int>());
    out.put("betWinner",gtipp["betWinner"]->index());
    out.put("reward",gtipp["reward"].get<int>());
    return out;
}