#include "email.h"
#include "../global.h"
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include "config.h"

using namespace tobilib;
using namespace network;

TokenEmail::TokenEmail():
    socket("new.phlhg.ch",443)
{
    // TODO socket timeoutoptionen
    log.prefix = "registrierungs-email: ";
    socket.log.parent = &log;
}

void TokenEmail::tick()
{
    socket.tick();

    while (!socket.events.empty())
    switch (socket.events.next())
    {
    case EndpointEvent::connected:
        send();
        break;
    case EndpointEvent::received:
        received += socket.read();
        break;
    case EndpointEvent::inactive:
        break;
    case EndpointEvent::closed:
        checkHttpResponse();
        break;
    }

    switch (socket.status())
    {
    case EndpointStatus::connecting:
        break;
    case EndpointStatus::connected:
        break;
    case EndpointStatus::closing:
        break;
    case EndpointStatus::closed:
        if (tasks.hasTask())
            begin_cycle();
        break;
    }
}

void TokenEmail::begin_cycle()
{
    Input& input = tasks.getCurrent();
    result.result = RESULT_SUCCESS;
    result.jsid = input.jsid;
    socket.connect();
}

void TokenEmail::send()
{
    Input& data = tasks.getCurrent();

    std::string key = get_config("../../config/emailkey.txt");
    if (key.empty())
    {
        log << "Der Emailschlüssel kann nicht gelesen werden." << std::endl;
        result.result = RESULT_SERVER_ERROR;
        socket.close();
    }

    h2rfp::JSObject reqdata;
    reqdata.put("sec",key);
    reqdata.put("name",data.name);
    reqdata.put("token",data.token);
    reqdata.put("email",data.email);
    std::stringstream httpbody;
    try {
        boost::property_tree::write_json(httpbody, reqdata, false);
    } catch (boost::property_tree::json_parser_error& e) {
        log << "Die Signup daten konnten nicht in JSON formattiert werden: " << e.what() << std::endl;
        result.result = RESULT_SERVER_ERROR;
        socket.close();
        return;
    }

    std::stringstream httpreq;
    httpreq << "GET /email.php HTTP/1.1\n"; // TODO neue adresse
    httpreq << "Host: new.phlhg.ch\n";
    httpreq << "Connection: close\n";
    httpreq << "Content-Length: " << httpbody.str().size() << "\n";
    httpreq << "\n";
    httpreq << httpbody.str();
    socket.write(httpreq.str());
}

void TokenEmail::checkHttpResponse()
{
    if (result.result != RESULT_SUCCESS)
    {
        tries = 0;
        end_cycle_error();
    }
    else if (received.find("EMAIL_SENT")!=std::string::npos)
    {
        tries = 0;
        end_cycle_success();
    }
    else if (++tries >= 3)
    {
        tries = 0;
        log << "email.php meldet fehler:" << std::endl;
        log << received << std::endl;
        result.result = RESULT_SERVER_ERROR;
        end_cycle_error();
    }
    
}

void TokenEmail::end_cycle_success()
{
    received.clear();
    Input& input = tasks.getCurrent();
    log << input.name << " ist dem Spiel beigetreten." << std::endl;
    tasks.finishCurrent(result);
}

void TokenEmail::end_cycle_error()
{
    // TODO Testen ob nutzer wieder entfernt wird wenn email nicht versandt wird
    received.clear();
    Input& input = tasks.getCurrent();
    data->database.list("User")[input.userid].erase();
    tasks.finishCurrent(result);
}