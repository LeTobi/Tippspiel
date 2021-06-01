#include "email.h"
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include "../misc/config.h"
#include "../main-data.h"

using namespace tobilib;
using namespace network;

void EmailTask::init()
{
    log.prefix = "Email task: ";
    log.parent = &maindata->log;
}

void EmailTask::tick()
{
    if (retry_timeout.due())
        retry_timeout.disable();

    if (socket==nullptr && tasks.hasTask() && !retry_timeout.is_enabled())
        on_new_task();

    if (socket!=nullptr)
    {
        socket->tick();

        while (!socket->events.empty())
        switch (socket->events.next())
        {
        case EndpointEvent::connected:
            on_connect();
            break;
        case EndpointEvent::received:
            received += socket->read();
            break;
        case EndpointEvent::inactive:
            break;
        case EndpointEvent::closed:
            break;
        }

        switch (socket->status())
        {
        case EndpointStatus::connecting:
            break;
        case EndpointStatus::connected:
            break;
        case EndpointStatus::closing:
            break;
        case EndpointStatus::closed:
            on_response();
            delete socket;
            socket = nullptr;
            break;
        }
    }
}

void EmailTask::send_http_request(const std::string& path, const h2rfp::JSObject& data)
{
    std::stringstream httpbody;
    try {
        boost::property_tree::write_json(httpbody, data, false);
    } catch (boost::property_tree::json_parser_error& e) {
        log << "Die Signup daten konnten nicht in JSON formattiert werden: " << e.what() << std::endl;
        EmailTask::Result res;
        res.success=false;
        res.msg = "Das Universum bricht zusammen: siehe logdatei";
        tasks.finishCurrent(res);
        socket->close();
        return;
    }

    std::stringstream httpreq;
    httpreq << "GET " << path << " HTTP/1.1\n";
    httpreq << "Host: new.phlhg.ch\n";
    httpreq << "Connection: close\n";
    httpreq << "Content-Length: " << httpbody.str().size() << "\n";
    httpreq << "\n"; // 1x '\n' aus JSON-parser
    httpreq << httpbody.str();
    socket->write(httpreq.str());
}

void EmailTask::send_register_mail()
{
    std::string key = get_config("../../config/email/key.txt");
    if (key.empty())
    {
        on_config_fail("key.txt");
        return;
    }
    std::string path = get_config("../../config/email/addr_register.txt");
    if (path.empty())
    {
        on_config_fail("addr_register.txt");
        return;
    }
    
    Input& params = tasks.getCurrent();
    h2rfp::JSObject reqdata;
    reqdata.put("sec",key);
    reqdata.put("name",params.user["name"].get<std::string>());
    reqdata.put("token",params.new_token);
    reqdata.put("email",params.user["email"].get<std::string>());
    if (!params.lang.empty())
        reqdata.put("lang",params.lang);
    send_http_request(path,reqdata);
}

void EmailTask::send_restore_mail()
{
    std::string key = get_config("../../config/email/key.txt");
    if (key.empty())
    {
        on_config_fail("key.txt");
        return;
    }
    std::string path = get_config("../../config/email/addr_restore_token.txt");
    if (path.empty())
    {
        on_config_fail("addr_restore_token.txt");
        return;
    }
    
    Input& params = tasks.getCurrent();
    h2rfp::JSObject reqdata;
    reqdata.put("sec",key);
    reqdata.put("name",params.user["name"].get<std::string>());
    reqdata.put("token",params.new_token);
    reqdata.put("email",params.user["email"].get<std::string>());
    if (!params.lang.empty())
        reqdata.put("lang",params.lang);
    send_http_request(path,reqdata);
}

void EmailTask::on_new_task()
{
    log << "Emailtask beginnt" << std::endl;
    std::string domain = get_config("../../config/email/domain.txt");
    if (domain.empty())
    {
        on_config_fail("domain.txt");
        return;
    }
    result.success = false;
    result.msg = "Es wurde keine Verbindung aufgebaut.";
    received.clear();
    socket = new SSL_Endpoint(domain,443);
    socket->log.parent = &log;
    socket->options.close_timeout = 5;
    socket->options.connect_timeout = 10;
    socket->options.handshake_timeout = 5;
    socket->options.inactive_warning = 5;
    socket->options.read_timeout = 10;
    socket->options.send_timeout = 8;
    socket->connect();
}

void EmailTask::on_connect()
{
    Input& data = tasks.getCurrent();
    result.success = true;
    result.msg.clear();
    switch (data.type) {
        case Type::registration:
            send_register_mail();
            break;
        case Type::restore_token:
            send_restore_mail();
            break;
    }
}

void EmailTask::on_config_fail(const char* file)
{
    log << file << " kann nicht gelesen werden" << std::endl;
    result.success = false;
    result.msg = "Server interner fehler. Benachrichtige die Entwickler";
    if (socket!=nullptr)
        socket->close();
    else
        on_response_error();
}

void EmailTask::on_response()
{
    if (result.success==false) {
        on_response_error();
        return;
    }
    if (received.find("EMAIL_SENT")!=std::string::npos) {
        on_response_success();
        return;
    }
    
    log << "server meldet fehler: " << received << std::endl;
    result.success = false;
    result.msg = "Die Email konnte nicht versendet werden.";
    on_response_error();
}

void EmailTask::on_response_success()
{
    log << "email an " << tasks.getCurrent().user["email"].get<std::string>() << " wurde gesendet" << std::endl;
    tries = 0;
    tasks.finishCurrent(result);
}

void EmailTask::on_response_error()
{
    if (++tries >= 3)
    {
        log << "Task wird abgebrochen" << std::endl;
        tasks.finishCurrent(result);
        tries=0;
    }
    else
    {
        log << "erneuter versuch in 10s" << std::endl;
        retry_timeout.set(10);
    }
}