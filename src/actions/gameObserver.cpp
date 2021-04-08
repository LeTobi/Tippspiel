#include "gameObserver.h"
#include "../main-data.h"
#include "../filters/all.h"
#include "../filters/gameTimeline.h"
#include "../misc/enums.h"
#include "../dataEdit/game.h"

void GameObserver::init()
{
    log.prefix = "GameObserver: ";
    log.parent = &maindata->log;
}

void GameObserver::tick()
{
    auto it = maindata->filters.timeline.running_begin;
    while (it != maindata->filters.timeline.running_end) {
        if ((**it).cluster["gameStatus"].get<int>() != GSTATUS_RUNNING) {
            log << "Spiel nr. " << (**it).cluster.index() << " wird aktiv" << std::endl;
            data_edit::set_game_status((**it).cluster,GSTATUS_RUNNING);
            return; // iteratoren werden ungültig
        }
        ++it;
    }
    for (auto gp: maindata->filters.timeline.pending_games) {
        if (gp->cluster["gameStatus"].get<int>() != GSTATUS_PENDING) {
            log << "Erwarte Resultate von Spiel nr. " << gp->cluster.index() << std::endl;
            data_edit::set_game_status(gp->cluster,GSTATUS_PENDING);
            return; // iteratoren werden ungültig
        }
    }
}