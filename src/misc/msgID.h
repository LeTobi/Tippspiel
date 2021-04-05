#ifndef TIPPSPIEL_MISC_MSGID_H
#define TIPPSPIEL_MISC_MSGID_H

#include <tobilib/database/database.h>

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
    hotGames
};

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