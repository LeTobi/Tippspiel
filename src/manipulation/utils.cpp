#include "utils.h"

Database::Cluster get_location(Database::Cluster user, std::string desc)
{
    if (desc.size()>100)
        desc.resize(100);
    for (Database::Cluster loc: data->database.list("Location"))
        if (loc["name"].get<std::string>() == desc)
            return loc;
    Database::Cluster out = data->database.list("Location").emplace();
    out["creator"].set( user );
    out["createdAt"].set( get_time() );
    out["name"].set( desc );
    return out;
}