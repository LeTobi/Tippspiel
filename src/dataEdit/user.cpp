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
    user["perm_groupCreate"].set( true );
    user["perm_console"].set( false );
    return user;
}

void data_edit::delete_user(tobilib::Database::Cluster user)
{
    if (user.reference_count()>0) {
        maindata->log << "Server-fehler: Es wurde versucht einen Nutzer zu entfernen, der verwendet wird: id " << user.index() << std::endl;
        return;
    }
    user.erase();
}

void data_edit::set_user_token(Database::Cluster user, const std::string& token)
{
    user["token"].set( token );
}

void data_edit::set_user_last_login(Database::Cluster user)
{
    user["lastlogin"].set( get_time() );
    global_message_update(user, WAIT_ENDLESS);
}

void data_edit::set_user_ban(Database::Cluster user, bool ban)
{
    user["banned"].set( ban );
    global_message_update(user, WAIT_SHORT);
}

void data_edit::set_user_lastrecovery(Database::Cluster user, Time t)
{
    user["lastrecovery"].set(t);
}

void data_edit::set_user_sync(Database::Cluster user, Time t)
{
    user["lastupdate"].set(t);
    global_message_update(user, WAIT_ENDLESS);
}