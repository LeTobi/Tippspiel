#include "all.h"
#include "../main-data.h"

using namespace tobilib;

void filters::tick_all()
{
    maindata->gametimeline.tick();
}

void filters::update(Database::Cluster cluster)
{
    
}

void filters::remove(Database::Cluster cluster)
{

}