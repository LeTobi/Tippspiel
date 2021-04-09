#include "gameTimeline.h"
#include "../main-data.h"
#include "../msgTracking.h"
#include "../misc/enums.h"

using namespace tobilib;

GameTimeline::Game::Game(Database::Cluster cl)
{
    cluster = cl;
    update();
}

GameTimeline::Game::Game(Time t)
{
    starttime = t;
    endtime = t;
}

void GameTimeline::Game::update()
{
    if (cluster.is_null())
        return;
    starttime = cluster["start"].get<int>();
    endtime = starttime + (45 + 15 + 45) * 60;
    switch (cluster["phase"].get<int>())
    {
    case GAMEPHASE_PENALTY:
        endtime += (5 + 10) * 60;
    case GAMEPHASE_OVERTIME:
        endtime += (5 + 15 + 5 + 15) * 60;
    case GAMEPHASE_NORMAL:
        break;
    }
}

bool GameTimeline::StartOrder::operator()(const Game* a, const Game* b)
{
    if (b==nullptr)
        return false;
    if (a==nullptr)
        return true;
    return a->starttime < b->starttime;
}

bool GameTimeline::FinishOrder::operator()(const Game* a, const Game* b)
{
    if (b==nullptr)
        return false;
    if (a==nullptr)
        return true;
    return a->endtime < b->endtime;
}

GameTimeline::~GameTimeline()
{
    for (auto& item: games)
        delete item.second;
}

void GameTimeline::init()
{
    for (Database::Cluster storedgame: maindata->storage.list("Game")) {
        Game* game = new Game(storedgame);
        games.insert({storedgame, game});
        startlist.insert(game);
        finishlist.insert(game);
    }
    set_indicators();
}

void GameTimeline::tick()
{
    Game now (get_time());
    bool new_running = upcoming_begin!=upcoming_end && has_started(&now,*upcoming_begin);
    bool new_finished = running_end!=running_begin && has_finished(&now,*running_begin);
    
    if (new_running || new_finished) {
        set_indicators();
        global_message_update(FilterID::games_running);
    }
    if (new_running) {
        global_message_update(FilterID::games_upcoming);
    }
    if (new_finished) {
        global_message_update(FilterID::games_finished);
    }
}

void GameTimeline::update(Database::Cluster cluster)
{
    if (cluster.type().name != "Game")
        return;

    Game now (get_time());
    Game* game;
    if (games.count(cluster)==0)
    {
        game = new Game(cluster);
        games.insert({cluster,game});
    }
    else
    {
        game = games.at(cluster);
    }

    bool running_before = is_running(&now,game);
    bool upcoming_before = hits_upcoming_horizon(&now,game);
    bool finished_before = hits_finished_horizon(&now,game);

    if (last_ignore!=nullptr && cluster == last_ignore->cluster) {
        // das sollte eigentlich nie eintreffen, da die ignorier-liste nicht vom server bearbeitet wird.
        // Aber Bearbeitungen von hand wären möglich
        auto it = finishlist.find(last_ignore);
        if (it==finishlist.begin())
            last_ignore = nullptr;
        else
            last_ignore = *(--it);
    }

    startlist.erase(game);
    finishlist.erase(game);
    game->update();
    startlist.insert(game);
    finishlist.insert(game);
    set_indicators();

    bool running_after = is_running(&now,game);
    bool upcoming_after = hits_upcoming_horizon(&now,game);
    bool finished_after = hits_finished_horizon(&now,game);

    if (running_before != running_after)
        global_message_update(FilterID::games_running);
    if (finished_before != finished_after)
        global_message_update(FilterID::games_finished);
    if (upcoming_before != upcoming_after)
        global_message_update(FilterID::games_upcoming);
}

void GameTimeline::remove(Database::Cluster cluster)
{
    //TODO
}

bool GameTimeline::hits_upcoming_horizon(const Game* now, const Game* game)
{
    if (upcoming_begin == upcoming_end)
        return !has_started(now,game);
    auto last_upcoming = upcoming_end;
    last_upcoming--;
    return !(StartOrder()(game,*upcoming_begin) || StartOrder()(*last_upcoming,game));
}

bool GameTimeline::hits_finished_horizon(const Game* now, const Game* game)
{
    if (finished_begin == finished_end)
        return has_finished(now,game);
    auto last_finished = finished_end;
    last_finished--;
    return !(FinishOrder()(game,*finished_begin) || FinishOrder()(*last_finished,game));
}

bool GameTimeline::is_running(const Game* now, const Game* game)
{
    return has_started(now,game) && !has_finished(now,game);
}

bool GameTimeline::has_started(const Game* now, const Game* game)
{
    return !StartOrder()(now,game);
}

bool GameTimeline::has_finished(const Game*now, const Game* game)
{
    return !FinishOrder()(now,game);
}

void GameTimeline::set_indicators()
{
    Game now (get_time());

    upcoming_begin = startlist.upper_bound(&now);
    upcoming_end = upcoming_begin;
    for (int i=0;i<5;i++)
        if (upcoming_end!=startlist.end())
            ++upcoming_end;

    finished_end = finishlist.upper_bound(&now);
    finished_begin = finished_end;
    for (int i=0;i<5;i++)
        if (finished_begin!=finishlist.begin())
            --finished_begin;

    running_begin = finished_end;
    running_end = running_begin;
    while (running_end!=finishlist.end() && is_running(&now,*running_end))
        ++running_end;


    // run through potential pending games
    FinishIterator it;
    if (last_ignore==nullptr)
        it = finishlist.begin();
    else
        it = ++finishlist.find(last_ignore);
    std::set<Game*> new_pendings;    
    bool ignore = true;
    bool has_updated = false;
    while(it!=finishlist.end() && has_finished(&now,*it))
    {
        if ((**it).cluster["gameStatus"].get<int>() == GSTATUS_ENDED)
        {
            if (ignore)
                last_ignore = *it;
        }
        else
        {
            new_pendings.insert(*it);
            if (pending_games.count(*it)==0)
                has_updated = true;
            ignore = false;
        }
        ++it;
    }
    if (pending_games.size() != new_pendings.size())
        has_updated=true;
    pending_games = new_pendings;
    if (has_updated)
        global_message_update(FilterID::games_pending);
}