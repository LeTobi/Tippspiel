#include <thread>

#include <tobilib/stringplus/stringplus.h>
#include <tobilib/general/timer.hpp>
#include "main-data.h"
#include "dataEdit/game.h"

using namespace tobilib;

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
        
        for (Database::Cluster game: maindata->storage.list("Game")) {
            data_edit::game_evaluate(game);
            std::cout << game.index() << std::endl;
        }

        std::cout << "Beendet" << std::endl;
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