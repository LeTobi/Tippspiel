#ifndef TIPPSPIEL_GAME_MANIPULATION_H
#define TIPPSPIEL_GAME_MANIPULATION_H

#include "../global.h"

Database::Cluster get_tipp(Database::Cluster user, Database::Cluster game);

void create_player(Client&, h2rfp::Message);
void make_tipp(Client&, h2rfp::Message);
void create_game(Client&, h2rfp::Message);
void report_game(Client&, h2rfp::Message);

#endif