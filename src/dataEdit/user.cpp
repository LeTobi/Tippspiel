#include "user.h"
#include "../misc/time.h"
#include "../msgTracking.h"
#include "../main-data.h"

using namespace tobilib;

Database::Cluster data_edit::create_user(const std::string& name, const std::string& email)
{
    Database::Cluster user = maindata->storage.list("User").emplace();
    user["name"].set( name );
    user["email"].set( email );
    user["registerdate"].set( get_time() );

    user["perm_eventAnnounce"].set( false );
    user["perm_eventReport"].set( false );
    user["perm_gameAnnounce"].set( false );
    user["perm_gameReport"].set( true );
    user["perm_console"].set( false );
    return user;
}

void data_edit::set_user_token(Database::Cluster user, const std::string& token)
{
    user["token"].set( token );
}

void data_edit::set_user_last_login(Database::Cluster user)
{
    user["lastlogin"].set( get_time() );
    global_message_update(user, WAIT_SHORT);
}

void data_edit::set_user_sync(Database::Cluster user, Time t)
{
    user["lastupdate"].set(t);
}