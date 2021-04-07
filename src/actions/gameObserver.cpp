#include "gameObserver.h"
#include "../main-data.h"
#include "../filters/all.h"
#include "../filters/gameTimeline.h"
#include "../misc/enums.h"
#include "../dataEdit/game.h"

void GameObserver::init()
{
    // nichts die bohne
}

void GameObserver::tick()
{
    GameTimeline::Iterator it = maindata->filters.timeline.next_game;
    Time now = get_time();
    Time new_horizon = 0;
    while (it != maindata->filters.timeline.timeline.begin()
        && (**it).starttime >= horizon)
    {
        int gamestate = (**it).cluster["gameStatus"].get<int>();
        if ((**it).starttime <= now)
        {
            // Spiel hat begonnen
            if (gamestate < GSTATUS_RUNNING)
                data_edit::set_game_status((**it).cluster,GSTATUS_RUNNING);
        }
        if ((**it).endtime > now)
        {
            // Spiel ist am laufen
            if (gamestate > GSTATUS_RUNNING)
                data_edit::set_game_status((**it).cluster,GSTATUS_RUNNING);
        }
        else
        {
            // Spiel ist vorbei
            if (gamestate < GSTATUS_PENDING)
                data_edit::set_game_status((**it).cluster,GSTATUS_PENDING);
        }
        if (gamestate != GSTATUS_ENDED)
        {
            // Hier ist ist eine Aktivität
            new_horizon = (**it).starttime;
        }
        it--;
    }
    horizon = new_horizon;
}