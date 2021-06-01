#include "group.h"
#include "../misc/response_util.h"
#include "../dataEdit/group.h"
#include "../main-data.h"

#include <tobilib/stringplus/stringplus.h>

using namespace tobilib;
using namespace h2rfp;

bool group_validate_name(const std::string& name)
{
    return name.size()>=3 && name.size()<100;
}

void msg_handler::group_create(Session& session,const tobilib::h2rfp::Message& msg)
{
    if (!check_login(session,msg) ||
        !check_permission(session,msg,"perm_groupCreate") ||
        !check_parameter(session,msg,"name"))
        return;

    std::string name = msg.data.get("name","");
    if (!group_validate_name(name)) {
        return_client_error(session,msg,"Name kann nicht gewaehlt werden.");
        return;
    }

    Database::Cluster group = data_edit::group_create(session.user,name);
    JSObject answer = make_result();
    answer.put("data.id", group.index() );
    return_result(session,msg,answer);
}

void msg_handler::group_rename(Session& session,const Message& msg)
{
    if (!check_login(session,msg) ||
        !check_parameter(session,msg,"name") ||
        !check_parameter(session,msg,"group"))
        return;

    Database::Cluster group = maindata->storage.list("Group")[msg.data.get("group",0)];
    if (group.is_null()) {
        return_client_error(session,msg,"Die Gruppe wurde nicht gefunden");
        return;
    }
    if (*group["admin"] != session.user) {
        return_client_error(session,msg,"group_rename ist nur fuer den Gruppenadmin");
        return;
    }

    std::string name = msg.data.get("name","");
    if (!group_validate_name(name)) {
        return_client_error(session,msg,"Der name kann nicht gewaehlt werden.");
        return;
    }

    data_edit::group_rename(group,name);
    return_result(session,msg,make_result());
}

void msg_handler::group_join(Session& session,const tobilib::h2rfp::Message& msg)
{
    if (!check_login(session,msg) ||
        !check_parameter(session,msg,"token"))
        return;

    std::string token = msg.data.get("token","");
    StringPlus index = StringPlus(token).split("-")[0];
    if (!index.isInt()) {
        return_result(session,msg,make_user_error(ERROR_GROUP_NOT_FOUND));
        return;
    }

    Database::Cluster group = maindata->storage.list("Group")[index.toInt()];
    if (group.is_null() || group["token"].get<std::string>()!=token) {
        return_result(session,msg,make_user_error(ERROR_GROUP_NOT_FOUND));
        return;
    }

    data_edit::group_add_user(group,session.user);
    JSObject answer = make_result();
    answer.put("data.id", group.index());
    return_result(session,msg,answer);
}

void msg_handler::group_reset_token(Session& session,const tobilib::h2rfp::Message& msg)
{
    if (!check_login(session,msg) ||
        !check_parameter(session,msg,"group"))
        return;
    
    Database::Cluster group = maindata->storage.list("Group")[msg.data.get("group",0)];
    if (group.is_null()) {
        return_client_error(session,msg,"Die Gruppe wurde nicht gefunden");
        return;
    }
    if (*group["admin"] != session.user) {
        return_client_error(session,msg,"group_reset_token ist nur fuer den Gruppenadmin");
        return;
    }
    data_edit::group_refresh_token(group);
    return_result(session,msg,make_result());
}

void msg_handler::group_leave(Session& session,const tobilib::h2rfp::Message& msg)
{
    if (!check_login(session,msg) ||
        !check_parameter(session,msg,"group"))
        return;

    Database::Cluster group = maindata->storage.list("Group")[msg.data.get("group",0)];
    if (group.is_null()) {
        return_client_error(session,msg,"Die Gruppe wurde nicht gefunden");
        return;
    }

    // Es ist nicht nötig, dass der Spieler tatsächlich in der Gruppe ist
    data_edit::group_remove_user(group, session.user);
    return_result(session,msg,make_result());
}