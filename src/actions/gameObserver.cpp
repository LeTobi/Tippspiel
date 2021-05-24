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
}