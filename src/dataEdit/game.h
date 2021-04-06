#ifndef TIPPSPIEL_DATAEDIT_GAME_H
#define TIPPSPIEL_DATAEDIT_GAME_H

#include <tobilib/database/database.h>
#include "../misc/time.h"

namespace data_edit
{
    tobilib::Database::Cluster get_game_tipp(
        tobilib::Database::Cluster user,
        tobilib::Database::Cluster game,
        bool force // erstellt den Tipp, falls er noch nicht existiert.
        );
    
    void set_game_tipp(
        tobilib::Database::Cluster tipp,
        int bet1,
        int bet2,
        tobilib::Database::Cluster winner,
        tobilib::Database::Cluster topscorer
        );

    tobilib::Database::Cluster get_location(
        tobilib::Database::Cluster user,
        const std::string& name);

    tobilib::Database::Cluster create_game();

    void announce_game(
        tobilib::Database::Cluster user,
        tobilib::Database::Cluster game,
        const Time start,
        tobilib::Database::Cluster location,
        tobilib::Database::Cluster event,
        const std::string& name,
        const std::string& stream,
        tobilib::Database::Cluster team1,
        tobilib::Database::Cluster team2,
        tobilib::Database::Cluster previousStage1,
        tobilib::Database::Cluster previousStage2
        );

    //void report_game(Client&, h2rfp::Message);
}

#endif