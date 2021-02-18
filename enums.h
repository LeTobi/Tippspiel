#ifndef TIPPSPIEL_ENUMS_H
#define TIPPSPIEL_ENUMS_H

#include <vector>
#include <string>

enum Tables {
    users,
    groups,
    players,
    teams,
    events,
    games,
    etipps,
    gtipps
};

enum RFPResult {
    success,
    user_err,
    client_err,
    server_err
};

enum ErrCode {
    NO_ERROR = 0,
    NOT_LOGGED_IN = 1,
    EMAIL_TAKEN = 2,
    USERNAME_TAKEN = 3,
    LOGIN_DENIED = 4
};

#endif