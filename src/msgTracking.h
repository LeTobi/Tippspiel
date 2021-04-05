#ifndef TIPPSPIEL_MSG_TRACKING_H
#define TIPPSPIEL_MSG_TRACKING_H

#include "misc/time.h"
#include "misc/filterID.h"
#include <tobilib/database/database.h>

void global_message_update(tobilib::Database::Cluster, Time);
void global_message_update(FilterID);

#endif