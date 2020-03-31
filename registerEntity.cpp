#include "registerEntity.h"

RegisterEntity::RegisterEntity()
{
    ready = true;
    name = -1;
}

int RegisterEntity::getName()
{
    return name;
}

void RegisterEntity::setName(unsigned name)
{
    this->name = name;
}

bool RegisterEntity::isReady()
{
    return ready;
}

void RegisterEntity::setReady(bool ready)
{
    this->ready = ready;
}