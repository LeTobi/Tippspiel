#include "push.h"
#include "../main-data.h"
#include "../dataEdit/game.h"
#include "../dataEdit/push.h"
#include <fstream>
#include <boost/property_tree/json_parser.hpp>
#include <unistd.h> // fork()
#include <cstdlib> // system()
#include <sys/wait.h> // waitpid()

const std::string INPUT_PATH = "../../push/input.json";
const std::string OUTPUT_PATH = "../../push/output.txt";
const std::string PUSH_CMD = "php ../../push/notifier.php " + INPUT_PATH + " " + OUTPUT_PATH + " > ../../push/log.txt 2> ../../push/err.txt";

using namespace tobilib;
using namespace h2rfp;

void PushTask::init()
{
    log.parent = &maindata->log;
}

void PushTask::tick()
{
    if (send_process != 0) {
        // from https://stackoverflow.com/questions/5278582/checking-the-status-of-a-child-process-in-c
        int status;
        pid_t result = waitpid(send_process, &status, WNOHANG);
        if (result == 0) {
            // Child still alive
        } else if (result == -1) {
            // Error
            throw tobilib::Exception("Da stimmt was mit dem Push Prozess nicht.","PushTask::tick()");
        } else {
            send_process = 0;
            get_send_result(status);
        }
    }
    else if (!out_data.empty()) {
        send();
    }
}

void PushTask::game_reminder(tobilib::Database::Cluster game)
{
    for (Database::Cluster sub: maindata->storage.list("PushSub"))
    {
        Database::Cluster tipp = data_edit::get_game_tipp(*sub["user"],game,false);

        JSObject data;
        data.put("name1", game["teams"][0]["short"].get<std::string>());
        data.put("name2", game["teams"][1]["short"].get<std::string>());
        data.put("location", game["location"]["name"].get<std::string>());
        data.put("tipped", !tipp.is_null());
        data.put("id",game.index());

        enqueue(sub,"GameStart",data);
    }
}

void PushTask::game_results(tobilib::Database::Cluster game)
{
    for (Database::Cluster sub: maindata->storage.list("PushSub"))
    {
        Database::Cluster tipp = data_edit::get_game_tipp(*sub["user"],game,false);
        if (tipp.is_null())
            continue;
        JSObject data;
        data.put("name1", game["teams"][0]["short"].get<std::string>());
        data.put("name2", game["teams"][1]["short"].get<std::string>());
        data.put("score1", game["scores"][0].get<int>());
        data.put("score2", game["scores"][1].get<int>());
        data.put("penalty1", game["penalty"][0].get<int>());
        data.put("penalty2", game["penalty"][1].get<int>());
        data.put("phase", game["phase"].get<int>());
        data.put("points", tipp["reward"].get<int>());
        data.put("id", game.index());
        
        enqueue(sub,"GameEnd",data);
    }
    send();
}

void PushTask::enqueue(tobilib::Database::Cluster sub, const std::string& type, tobilib::h2rfp::JSObject data)
{
    JSObject entry;
    entry.put("subscription.endpoint", sub["endpoint"].get<std::string>());
    entry.put("subscription.keys.p256dh", sub["publickey"].get<std::string>());
    entry.put("subscription.keys.auth",sub["auth"].get<std::string>());
    entry.put("type",type);
    entry.put_child("data",data);
    out_data.push_back(entry);
}

void PushTask::send()
{
    if (send_process != 0)
        return;

    std::fstream file (INPUT_PATH,std::fstream::out);
    file << "[";
    for (int i=0;i<out_data.size();i++) {
        boost::property_tree::write_json(file,out_data[i],false);
        file.seekp(-1,std::fstream::end);
        if (i!=out_data.size()-1)
            file << ",";
    }
    file << "]";
    file.close();
    out_data.clear();

    send_process = fork();
    if (send_process == 0)
    {
        int code = system(PUSH_CMD.c_str());
        exit(code);
    }
}

void PushTask::get_send_result(int status)
{
    if (!WIFEXITED(status)) {
        log << "FEHLER: Der Push-Prozess wurde unfreiwillig beendet." << std::endl;
        return;
    }
    if (!WEXITSTATUS(status) != 0) {
        log << "FEHLER: Der Push-Prozess signalisiert einen Fehler" << std::endl;
        return;
    }

    log << "Eine Push benachrichtigung war erfolgreich." << std::endl;
    std::fstream file (OUTPUT_PATH,std::fstream::in);
    std::string endpoint;
    while (file >> endpoint)
    {
        Database::Cluster sub = data_edit::get_subscription(endpoint);
        if (sub.is_null())
            log << "Es wurde versucht ein Endpoint zu entfernen und er wurde nicht gefunden" << std::endl;
        else
            log << "entferne benachrichtigung von " << sub["user"]["name"].get<std::string>() << std::endl;
        data_edit::remove_push(endpoint);
    }
    file.close();
}