#include "timeline_game.h"
#include "../main-data.h"
#include "../msgTracking.h"

using namespace filters;
using namespace tobilib;

GameTimeline::Game::Game(Database::Cluster cl)
{
    cluster = cl;
    update();
}

GameTimeline::Game::Game(Time t)
{
    starttime = t;
    endtime = starttime + 90*60;
}

void GameTimeline::Game::update()
{
    if (cluster.is_null())
        return;
    starttime = cluster["start"].get<int>();
    endtime = starttime + 90*60; // hier könnte ihre Laufzeit stehen
}

bool GameTimeline::TimeOrder::operator()(const Game* a, const Game* b)
{
    if (b==nullptr)
        return false;
    if (a==nullptr)
        return true;
    return a->starttime < b->starttime;
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
        timeline.insert(game);
    }
    set_indicators();
}

void GameTimeline::tick()
{
    Game now (get_time());
    if (!TimeOrder()(&now,*most_current)) {
        global_message_update(FilterID::currentGames);
        set_indicators();
    }
}

void GameTimeline::update(Database::Cluster cluster)
{
    if (cluster.type().name != "Game")
        return;

    Game* game;
    if (games.count(cluster)==0) {
        game = new Game(cluster);
        games.insert({cluster,game});
    } else {
        game = games.at(cluster);
        timeline.erase(game);
        game->update();
    }

    if (TimeOrder()(*past_horizon,game) && TimeOrder()(game,*future_horizon)) {
        global_message_update(FilterID::currentGames);
        set_indicators();
    }
}

void GameTimeline::remove(Database::Cluster cluster)
{
    //TODO
}

void GameTimeline::set_indicators()
{
    Game now (get_time());
    most_current = timeline.upper_bound(&now);
    future_horizon = most_current;
    past_horizon = most_current;
    for (int i=0;i<5;i++)
        if (future_horizon!=timeline.end())
            ++future_horizon;
    for (int i=0;i<6;i++)
        if (past_horizon!=timeline.begin())
            --past_horizon;
}