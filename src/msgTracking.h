#ifndef TIPPSPIEL_MSG_TRACKING_H
#define TIPPSPIEL_MSG_TRACKING_H

#include <tobilib/database/database.h>
#include "misc/msgID.h"
#include "misc/time.h"
#include "updateTracker.h" // wegen WAIT_ konstanten

void global_message_delete(tobilib::Database::Cluster);
void global_message_update(tobilib::Database::Cluster, Time);
void global_message_update(FilterID);

#endif