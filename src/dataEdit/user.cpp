#include "user.h"

using namespace tobilib;

void data_edit::set_user_sync(Database::Cluster user, Time t)
{
    user["lastupdate"].set(t);
}