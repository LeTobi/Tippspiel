#include "gameobserver.h"
#include "../global.h"

const Time GameObserver::DURATION = 90*60*60;

GameObserver::GameObserver()
{ }

void GameObserver::setup()
{
    Time now = get_time();
    for (Game game: data->database.list("Game"))
    {
        KnownGame kg (game);
        knowngames.insert(std::make_pair(game,kg));
        timeline.insert(kg);
        int status = game["gameStatus"].get<int>();
        if (status == GSTATUS_ENDED)
            return;
        if (kg.starttime > now)
            game["gameStatus"].set( GSTATUS_UPCOMING );
        else if (kg.endtime < now)
            game["gameStatus"].set( GSTATUS_PENDING );
        else
            game["gameStatus"].set( GSTATUS_RUNNING );
    }
    get_next_game();
}

std::vector<GameObserver::Game> GameObserver::get_upcoming()
{
    TimeSorter::iterator next = timeline.find(next_start);
    std::vector<Game> out;
    for (int i=0;i<5;i++)
    {
        if (next==timeline.end())
            break;
        out.push_back(next->game);
        ++next;
    }
    return out;
}

std::vector<GameObserver::Game> GameObserver::get_finished()
{
    TimeSorter::iterator next = timeline.find(next_start);
    std::vector<Game> out;
    for (int i=0;i<5;i++)
    {
        --next;
        if (next==timeline.end())
            break;
        out.push_back(next->game);
        if (next==timeline.begin())
            break;
    }
    return out;
}

void GameObserver::update(Game game)
{
    if (knowngames.count(game) == 0)
    {
        KnownGame kg (game);
        knowngames.insert(std::make_pair(game,kg));
        timeline.insert(kg);
        get_next_game();
        return;
    }
    KnownGame& kg = knowngames[game];
    if (kg.starttime != game["start"].get<int>())
    {
        timeline.erase(kg);
        kg.starttime = game["start"].get<int>();
        timeline.insert(kg);
        get_next_game();
    }
}

void GameObserver::tick()
{
    Time now = get_time();
    if (next_start.starttime > 0 && next_start.starttime <= now) {
        // TODO sonstwasnoch
        data->virtualdb.update(next_start.game, VirtualDB::WAIT_NOT);
    }
    if (next_end.endtime > 0 && next_end.endtime <= now) {
        // TODO sonstwasnoch
        data->virtualdb.update(next_end.game, VirtualDB::WAIT_NOT);
    }
}

GameObserver::KnownGame::KnownGame()
{ }

GameObserver::KnownGame::KnownGame(const Game& g) {
    game = g;
    starttime = g["start"].get<int>();
    endtime = starttime + DURATION;
}

bool GameObserver::TimeOrder::operator() (const KnownGame& a, const KnownGame& b) const
{
    if (a.starttime < b.starttime)
        return true;
    if (a.starttime > b.starttime)
        return false;
    return DataOrder()(a.game,b.game);
}

bool GameObserver::DataOrder::operator() (const Game& a, const Game& b) const
{
    return a.index() < b.index();
}

void GameObserver::get_next_game()
{
    Time now = get_time();
    for (auto& kg: timeline)
        if (now<kg.starttime) {
            next_start = kg;
            break;
        }
    for (auto& kg: timeline)
        if (now<kg.endtime) {
            next_end = kg;
            break;
        }
}