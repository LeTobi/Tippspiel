#ifndef TIPPSPIEL_MSG_TRACKING_H
#define TIPPSPIEL_MSG_TRACKING_H

#include "misc/time.h"
#include <tobilib/database/database.h>
#include "misc/msgID.h"

void global_message_update(tobilib::Database::Cluster, Time);
void global_message_update(FilterID);

#endif