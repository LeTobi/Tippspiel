#include <iostream>
#include <tobilib/database/database.h>
#include <manipulate.h>

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
    manipulate();
    std::cout << "Programm beendet." << std::endl;
    return 0;
}