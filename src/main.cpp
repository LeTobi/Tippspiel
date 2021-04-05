#include <thread>

#include <tobilib/stringplus/stringplus.h>
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

        while (true)
        {
            std::this_thread::yield();

            maindata->updateTracker.tick();
            for (Session& cli: maindata->sessions)
                cli.tick();
            tasks::tick_all();
            filters::tick_all();
            actions::tick_all();

            if (!maindata->storage.is_good())
            {
                maindata->log << "Programm beendet wegen fehler in der Datenbank" << std::endl;
                break;
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