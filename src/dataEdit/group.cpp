#include "group.h"
#include "../main-data.h"
#include "../misc/time.h"
#include "../msgTracking.h"

#include <tobilib/stringplus/stringplus.h>

using namespace tobilib;

Database::Cluster data_edit::group_create(tobilib::Database::Cluster admin, std::string name)
{
    FlagRequest lock = maindata->storage.begin_critical_operation();
        Database::Cluster group = maindata->storage.list("Group").emplace();
        group["createdAt"].set( get_time() );
        group["creator"].set( admin );
        group["namegiver"].set( admin );
        group["admin"].set( admin );
        group["users"].emplace().set( admin );
        group["name"].set( name );
        admin["groups"].emplace().set( group );
        group_refresh_token(group);
    maindata->storage.end_critical_operation(lock);

    maindata->log << "Gruppe " << group.index() << " erstellt: " << name << std::endl;

    global_message_update(group,WAIT_LONG);
    global_message_update(admin,WAIT_LONG);

    return group;
}

void data_edit::group_rename(tobilib::Database::Cluster group, std::string name)
{
    maindata->log << "Gruppe " << group.index() << " wird umbenannt: " << group["name"].get<std::string>() << " -> " << name << std::endl;
    group["name"].set( name );
    group["namegiver"].set( *group["admin"] );

    global_message_update(group,WAIT_LONG);
}

void data_edit::group_refresh_token(tobilib::Database::Cluster group)
{
    maindata->log << "Neues Token fuer Gruppe " << group.index() << " (" << group["name"].get<std::string>() << ")" << std::endl;
    StringPlus token = StringPlus::random(10);
    token.insert(0,std::to_string(group.index())+"-");
    group["token"].set( token.toString() );
}

void data_edit::group_add_user(tobilib::Database::Cluster group, tobilib::Database::Cluster user)
{
    maindata->log << user["name"].get<std::string>() << " tritt der Gruppe \"" << group["name"].get<std::string>() << "\" bei" << std::endl;
    for (Database::Member mem: group["users"])
        if (*mem == user)
            return;
    FlagRequest lock = maindata->storage.begin_critical_operation();
        group["users"].emplace().set(user);
        user["groups"].emplace().set(group);
    maindata->storage.end_critical_operation(lock);

    global_message_update(group,WAIT_LONG);
    global_message_update(user,WAIT_LONG);
}

void data_edit::group_remove_user(tobilib::Database::Cluster group, tobilib::Database::Cluster user)
{
    maindata->log << user["name"].get<std::string>() << " tritt aus der Gruppe \"" << group["name"].get<std::string>() << "\" aus" << std::endl;
    FlagRequest lock = maindata->storage.begin_critical_operation();
        group["users"].erase( user );
        user["groups"].erase( group );

        if (group["users"].begin() == group["users"].end())
        {
            maindata->log << "Gruppe " << group.index() << " (" << group["name"].get<std::string>() << ") ist leer und wird entfernt" << std::endl;
            group.erase();
        }
        else if (*group["admin"] == user) {
            Database::Cluster new_admin = user;
            Database::MemberIterator last_member = group["users"].begin();
            while (last_member!=group["users"].end()) {
                new_admin = **last_member;
                ++last_member;
            }
            group["admin"].set( new_admin );
            maindata->log << "Gruppe " << group.index() << " (" << group["name"].get<std::string>() << ") hat neuen Admin: " << new_admin["name"].get<std::string>() << std::endl;
        }
    maindata->storage.end_critical_operation(lock);

    global_message_update(user,WAIT_LONG);
    global_message_update(group,WAIT_LONG);
}