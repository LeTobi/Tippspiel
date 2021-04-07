#include "all.h"
#include "gameTimeline.h"

using namespace tobilib;

DatabaseFilters::DatabaseFilters():
    timeline(*new GameTimeline())
{ }

DatabaseFilters::~DatabaseFilters()
{
    delete &timeline;
}

void DatabaseFilters::init()
{
    timeline.init();
}

void DatabaseFilters::tick()
{
    timeline.tick();
}

void DatabaseFilters::update(Database::Cluster cluster)
{
    timeline.update(cluster);
}

void DatabaseFilters::remove(Database::Cluster cluster)
{
    timeline.remove(cluster);
}