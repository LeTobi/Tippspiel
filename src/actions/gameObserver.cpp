#include "gameObserver.h"
#include "../main-data.h"
#include "../filters/all.h"
#include "../filters/gameTimeline.h"
#include "../misc/enums.h"
#include "../dataEdit/game.h"
#include "../tasks/all.h"
#include "../tasks/push.h"

const Time MAX_DIST_NORMAL = 60*60;
const Time MAX_DIST_HOT = 5*60*60;
const Time MIN_DIST = MAX_DIST_NORMAL - 10;

using namespace tobilib;

void GameObserver::init()
{
    log.prefix = "GameObserver: ";
    log.parent = &maindata->log;
}

void GameObserver::tick()
{
    Time now = get_time();
    for (GameTimeline::Game* game: maindata->filters.timeline.upcoming) {
        if (hits_time_window(now,game->starttime,MAX_DIST_NORMAL,MIN_DIST)) {
            send_reminders();
            break;
        }
    }
    for (GameTimeline::Game* game: maindata->filters.timeline.running) {
        if (game->cluster["gameStatus"].get<int>() != GSTATUS_RUNNING) {
            log << "Spiel nr. " << game->cluster.index() << " beginnt" << std::endl;
            data_edit::set_game_status(game->cluster,GSTATUS_RUNNING);
            return; // iteratoren werden ungültig
        }
    }
    for (GameTimeline::Game* game: maindata->filters.timeline.pending) {
        if (game->cluster["gameStatus"].get<int>() != GSTATUS_PENDING) {
            log << "Erwarte Resultate von Spiel nr. " << game->cluster.index() << std::endl;
            data_edit::set_game_status(game->cluster,GSTATUS_PENDING);
            return; // iteratoren werden ungültig
        }
    }
    
    for (int i = recent_push.size()-1; i>=0; i--)
    {
        if (now - recent_push[i].time > MAX_DIST_HOT - MIN_DIST + 10) {
            log << "DEBUG: recent-push entfernt (Spiel nr. " << recent_push[i].game.index() << ")" << std::endl;
            recent_push.erase(recent_push.begin()+i);
        }
    }
}

void GameObserver::send_reminders()
{
    Time now = get_time();
    for (GameTimeline::Game* game: maindata->filters.timeline.upcoming) {
        if (!hits_time_window(now,game->starttime,MAX_DIST_HOT,MIN_DIST))
            continue;
        
        if (is_already_pushed(game->cluster))
            continue;

        log << "Sende erinnerung an Spiel nr. " << game->cluster.index() << std::endl;
        maindata->tasks.push.game_reminder(game->cluster);
        recent_push.push_back({now,game->cluster});
    }
}

bool GameObserver::is_already_pushed(Database::Cluster game)
{
    for (PushInfo& info: recent_push)
        if (info.game == game)
            return true;
    return false;
}

bool GameObserver::hits_time_window(Time now, Time start, Time max_dist, Time min_dist) const
{
    Time diff = start-now;
    return diff>=min_dist && diff<=max_dist;
}