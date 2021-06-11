#include "push.h"
#include "../main-data.h"
#include "../misc/time.h"

using namespace tobilib;

tobilib::Database::Cluster data_edit::get_subscription(const std::string& endpoint)
{
    Database::Cluster sub;
    for (Database::Cluster s: maindata->storage.list("PushSub"))
        if (s["endpoint"].get<std::string>() == endpoint)
            return s;
    return Database::Cluster();
}

tobilib::Database::Cluster data_edit::add_push(
        tobilib::Database::Cluster user,
        const std::string& endpoint,
        const std::string& p256dh,
        const std::string& auth)
{
    Database::Cluster sub = get_subscription(endpoint);
    if (sub.is_null())
        sub = maindata->storage.list("PushSub").emplace();
    sub["createdAt"].set( get_time() );
    sub["user"].set( user );
    sub["endpoint"].set( endpoint );
    sub["auth"].set( auth );
    sub["publickey"].set( p256dh );

    return sub;
}

bool data_edit::remove_push(const std::string& endpoint)
{
    Database::Cluster sub = get_subscription(endpoint);
    if (sub.is_null())
        return false;
    sub.erase();
    return true;
}