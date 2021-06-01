#include <thread>

#include <tobilib/stringplus/stringplus.h>
#include <tobilib/general/timer.hpp>
#include "main-data.h"

MainData* maindata = nullptr;

int main(int argc, const char** args)
{
    int port = 15320;
    if (argc>=2) {
        tobilib::StringPlus portstr = args[1];
        if (!portstr.isInt()) {
            std::cout << "ungueltiger Port" << std::endl;
            return 0;
        }
        port = portstr.toInt();
    }
    maindata = new MainData();

    try {

        maindata->init(port);
        maindata->log << "Server wurde gestartet" << std::endl;
        maindata->log << "Port: " << port << std::endl;

        while (true)
        {
            maindata->tick();

            if (!maindata->storage.is_good())
            {
                maindata->log << "Programm beendet wegen fehler in der Datenbank" << std::endl;
                break;
            }

            switch (maindata->workstate) {
            case WORK_BUSY:
                std::this_thread::yield();
                break;
            case WORK_BACKGROUND:
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                break;
            case WORK_STANDBY:
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            default:
                throw tobilib::Exception(std::string("Unbekannter workstate: ")+std::to_string(maindata->workstate),"main()");
            }
        }
    }
    catch(tobilib::Exception& err)
    {
        maindata->log.print_time=false;
        maindata->log << err.what() << std::endl;
    }
    catch (std::exception& err)
    {
        maindata->log << err.what() << std::endl;
    }

    delete maindata;
    return 0;
}