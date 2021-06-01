#ifndef TIPPSPIEL_ENUMS_H
#define TIPPSPIEL_ENUMS_H

#include <vector>
#include <string>

enum {
    ESTATUS_UPCOMING,
    ESTATUS_RUNNING,
    ESTATUS_PENDING,
    ESTATUS_ENDED
};

enum {
    GSTATUS_UPCOMING,
    GSTATUS_RUNNING,
    GSTATUS_PENDING,
    GSTATUS_ENDED
};

enum {
    GAMEPHASE_NORMAL,
    GAMEPHASE_OVERTIME,
    GAMEPHASE_PENALTY
};

enum {
    RESULT_SUCCESS,
    RESULT_USER_ERROR,
    RESULT_CLIENT_ERROR,
    RESULT_SERVER_ERROR
};

enum {
    TIPPKAT_WRONG,
    TIPPKAT_TEAM,
    TIPPKAT_DIFF,
    TIPPKAT_EXACT
};

enum ErrCode {
    ERROR_NONE = 0,
    ERROR_NOT_LOGGEDIN      = 1,
    ERROR_EMAIL_TAKEN       = 2,  // {email} ist bereits belegt
    ERROR_USERNAME_TAKEN    = 3,  // {name} wird bereits genutzt
    ERROR_LOGIN_DENIED      = 4,  // Login fehlgeschlagen
    ERROR_INVALID_NAME      = 5,  // {name} kann nicht gewählt werden (für Player)
    ERROR_GAMETIPPS_LOCKED  = 6,  // Tipps sind deaktiviert
    ERROR_ALREADY_LOGGED_IN = 7,  // Beteits eingeloggt
    ERROR_ALREADY_RUNNING   = 8,  // Der Prozess läuft bereits
    ERROR_EMAIL_NOT_FOUND   = 9,  // {email} wurde nicht gefunden
    ERROR_BANNED            = 10, // Nutzer ist durch Admins vom Spiel ausgeschlossen
    ERROR_COOLDOWN          = 11, // Die Funktion ist erst wieder in {time} verfügbar
    ERROR_GROUP_NOT_FOUND   = 12  // Der Beitrittscode ist ungültig
};

#endif