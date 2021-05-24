#include "gameTimeline.h"
#include "../main-data.h"
#include "../msgTracking.h"
#include "../misc/enums.h"

const int UPCOMING_SIZE = 5;
const int FINISHED_SIZE = 5;

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

bool GameTimeline::StartOrder::operator()(const Game* a, const Game* b) const
{
    if (b==nullptr)
        return false;
    if (a==nullptr)
        return true;
    if (a->starttime == b->starttime)
        return a->cluster < b->cluster;
    return a->starttime < b->starttime;
}

bool GameTimeline::FinishOrder::operator()(const Game* a, const Game* b) const
{
    if (b==nullptr)
        return false;
    if (a==nullptr)
        return true;
    if (a->endtime == b->endtime)
        return a->cluster < b->cluster;
    return a->endtime < b->endtime;
}

GameTimeline::~GameTimeline()
{
    for (auto& item: games)
        delete item.second;
}

void GameTimeline::init()
{
    Game now (get_time());
    for (Database::Cluster storedgame: maindata->storage.list("Game")) {
        Game* game = new Game(storedgame);
        games.insert({storedgame, game});
        
        if (!has_begun(&now,game))
            future.insert(game);
        else if (!has_finished(&now,game))
            running.insert(game);
        else if (game->cluster["gameStatus"].get<int>()!=GSTATUS_ENDED)
            pending.insert(game);
        else
            finished.insert(game);
    }

    Startlist::iterator upcoming_start = future.begin();
    Startlist::iterator upcoming_end = future.begin();
    Finishlist::reverse_iterator finished_oldest = finished.rbegin();

    for (int i=0;i<UPCOMING_SIZE;i++)
        if (upcoming_end != future.end())
            ++upcoming_end;
    for (int i=0;i<FINISHED_SIZE;i++)
        if (finished_oldest != finished.rend())
            ++finished_oldest;

    upcoming.insert(upcoming_start,upcoming_end);
    future.erase(upcoming_start,upcoming_end);
    finished.erase(finished.begin(),finished_oldest.base());
}

void GameTimeline::tick()
{
    Game now (get_time());

    if (!upcoming.empty() && has_begun(&now,*upcoming.begin()))
    {
        running.insert(*upcoming.begin());
        upcoming.erase(upcoming.begin());
        if (!future.empty()) {
            upcoming.insert(*future.begin());
            future.erase(future.begin());
        }
        global_message_update(FilterID::games_running);
        global_message_update(FilterID::games_upcoming);
    }

    if (!running.empty() && has_finished(&now,*running.begin()))
    {
        pending.insert(*running.begin());
        running.erase(running.begin());
        global_message_update(FilterID::games_running);
        global_message_update(FilterID::games_pending);
    }
}

bool GameTimeline::has_begun(Game* now, Game* game)
{
    return !startorder(now,game);
}

bool GameTimeline::has_finished(Game* now, Game* game)
{
    return !finishorder(now,game);
}

void GameTimeline::update(Database::Cluster cluster)
{
    if (cluster.type().name != "Game")
        return;

    // interne referenz finden
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

    // alte infos entfernen
    if (future.count(game)>0) {
        future.erase(game);
    }
    if (upcoming.count(game)>0) {
        upcoming.erase(game);
        global_message_update(FilterID::games_upcoming);
    }
    if (running.count(game)>0) {
        running.erase(game);
        global_message_update(FilterID::games_running);
    }
    if (pending.count(game)>0) {
        pending.erase(game);
        global_message_update(FilterID::games_pending);
    }
    if (finished.count(game)>0) {
        finished.erase(game);
        global_message_update(FilterID::games_finished);
    }

    // neue infos hinzufügen
    game->update();

    if (upcoming.size()>=UPCOMING_SIZE && !startorder(game,*upcoming.rbegin())) {
        // future
        future.insert(game);
    }
    else if (!has_begun(&now,game)) {
        // upcoming
        upcoming.insert(game);
        if (upcoming.size()>UPCOMING_SIZE)
        {
            future.insert(*upcoming.rbegin());
            upcoming.erase(--upcoming.end());
        }
        global_message_update(FilterID::games_upcoming);
    }
    else if (!has_finished(&now,game))
    {
        // running
        running.insert(game);
        global_message_update(FilterID::games_running);
    }
    else if (finished.size()>=FINISHED_SIZE && finishorder(game,*finished.begin())) {
        // beyond finished
    }
    else if (game->cluster["gameStatus"].get<int>() == GSTATUS_ENDED)
    {
        // finished
        finished.insert(game);
        if (finished.size()>FINISHED_SIZE)
            finished.erase(finished.begin());
        global_message_update(FilterID::games_finished);
    }
    else
    {
        // pending
        pending.insert(game);
        global_message_update(FilterID::games_pending);
    }
}

void GameTimeline::remove(Database::Cluster cluster)
{
    //TODO
}