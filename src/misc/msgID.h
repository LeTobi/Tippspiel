#ifndef TIPPSPIEL_MISC_MSGID_H
#define TIPPSPIEL_MISC_MSGID_H

#include <tobilib/database/database.h>

// identifiziert Daten von Filtern
// Daten die nicht im Datei-Speicher vorhanden sind
enum class FilterID {
    currentGames // timeline_game
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
    gamePlayers
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