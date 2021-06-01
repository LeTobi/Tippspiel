#include <iostream>
#include <tobilib/database/database.h>
#include "manipulate.h"

using namespace tobilib;

Database database;

int main()
{
    std::cout << "Stelle sicher, dass das Tippspiel nicht läuft!" << std::endl;
    std::cout << "weiter mit 'ja'" << std::endl;
    std::string inp;
    std::getline(std::cin,inp);
    if (inp != "ja")
        return 0;

    std::cout << "bearbeite Datenbank..." << std::endl;
    database.setPath("../../data/");
    database.init();
    database.open();
    if (!database.is_good()) {
        std::cout << "Fehler beim initiieren der Datenbank" << std::endl;
        return 0;
    }

    manipulate();
    if (database.is_good())
        std::cout << "Der Eingriff ist zu ende." << std::endl;
    else
        std::cout << "Es ist ein Fehler in der Datenbank aufgetreten." << std::endl;
    return 0;
}