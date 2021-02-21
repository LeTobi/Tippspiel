#ifndef TIPPSPIEL_SYNC_H
#define TIPPSPIEL_SYNC_H

#include "../global.h"

h2rfp::JSObject custom_client_data(Client&, VirtualDB::Entry);

h2rfp::JSObject private_gtipp(Database::Cluster&);
h2rfp::JSObject private_etipp(Database::Cluster&);

#endif