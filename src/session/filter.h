#ifndef TIPPSPIEL_REQUEST_FILTER_H
#define TIPPSPIEL_REQUEST_FILTER_H

#include "../global.h"

void filter_hot_games(Client&, h2rfp::Message);
void suggest_players(Client&, h2rfp::Message);
void suggest_locations(Client&, h2rfp::Message);

#endif