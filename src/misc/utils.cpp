#include "utils.h"

using namespace tobilib;

bool list_contains_cluster(Database::Member list, Database::Cluster item)
{
    for (Database::Member mem: list)
        if (*mem == item)
            return true;
    return false;
}