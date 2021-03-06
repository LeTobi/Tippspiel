#ifndef TIPPSPIEL_MISC_MSGID_H
#define TIPPSPIEL_MISC_MSGID_H

#include <tobilib/database/database.h>

// identifiziert Daten von Filtern
// Daten die nicht im Datei-Speicher vorhanden sind
enum class FilterID {
    games_upcoming,
    games_running,
    games_pending,
    games_finished,
    player_suggestions
};

// identifiziert die Art von server-internen Nachrichtenpaketen
enum class MsgType {
    none,
    user,
    group,
    player,
    team,
    event,
    game,
    eventTipp,
    gameTipp,
    locations,
    hotGames,
    player_suggestions,
    eventRank
};

// identifiziert server-interne Nachrichtenpakete
struct MsgID {
    MsgType type = MsgType::none;
    unsigned int arg0 = 0;

    MsgID(MsgType, unsigned int);
    MsgID(MsgType);
    bool operator==(const MsgID&) const;
    bool operator!=(const MsgID&) const;
    bool operator<(const MsgID&) const;
};

#endif