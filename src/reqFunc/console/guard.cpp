#include "guard.h"
#include "util.h"
#include "../../misc/response_util.h"
#include "../../main-data.h"
#include "../../dataEdit/user.h"

using namespace tobilib;
using namespace h2rfp;

void cmd_guard(Session& session, const Message& msg, const std::string& in)
{
    std::stringstream input (in);
    std::string cmd;
    input >> cmd;
    if (!input) {
        cmd_echo(session,msg,"gard: erwarte anweisung");
        return;
    }
    if (cmd=="watch")
    {
        std::map<Database::Cluster,int> active_users;
        int sum_active = 0;
        int sum_pending = 0;
        for (Session& sess: maindata->sessions) {
            if (sess.status == Session::Status::active)
            {
                sum_active++;
                if (active_users.count(sess.user)==0)
                    active_users[sess.user] = 1;
                else
                    active_users[sess.user]++;
            }
            if (sess.status == Session::Status::cleanup)
            {
                sum_pending++;
            }
        }
        std::stringstream answer;
        answer << maindata->sessions.size() << " sessions:\n";
        answer << "- " << sum_active << " aktiv (" << active_users.size() << " Nutzer)\n";
        answer << "- " << sum_pending << " am beenden\n";
        answer << "- " << (maindata->sessions.size() - sum_active - sum_pending) << " bereit\n";
        answer << "Nutzer:";
        for (auto& item: active_users)
        {
            Database::Cluster cli_user = item.first;
            if (cli_user.is_null())
                answer << "\n- " << item.second << "x not logged in";
            else
                answer << "\n- " << item.second << "x id " << cli_user.index() << " (" << cli_user["name"].get<std::string>() << ")";
        }
        cmd_echo(session,msg,answer.str());
        return;
    }
    else if (cmd=="ban")
    {
        unsigned int index;
        input >> index;
        if (!input)
        {
            cmd_echo(session,msg,"ban: erwarte user id");
            return;
        }
        Database::Cluster user = maindata->storage.list("User")[index];
        if (user.is_null())
        {
            cmd_echo(session,msg,"ban: user nicht gefunden");
            return;
        }
        if (user["banned"].get<bool>())
        {
            cmd_echo(session,msg,user["name"].get<std::string>() + " ist bereits gebannt");
            return;
        }
        data_edit::set_user_ban(user,true);
        cmd_echo(session,msg,user["name"].get<std::string>() + " wurde gebannt");
        return;
    }
    else if (cmd=="pardon")
    {
        unsigned int index;
        input >> index;
        if (!input)
        {
            cmd_echo(session,msg,"pardon: erwarte user id");
            return;
        }
        Database::Cluster user = maindata->storage.list("User")[index];
        if (user.is_null())
        {
            cmd_echo(session,msg,"pardon: user nicht gefunden");
            return;
        }
        if (!user["banned"].get<bool>())
        {
            cmd_echo(session,msg,user["name"].get<std::string>() + " ist nicht gebannt");
            return;
        }
        data_edit::set_user_ban(user,false);
        cmd_echo(session,msg,user["name"].get<std::string>() + " wurde entbannt");
        return;
    }
    else
    {
        cmd_echo(session,msg,"unbekannte anweisung an guard");
        return;
    }

}