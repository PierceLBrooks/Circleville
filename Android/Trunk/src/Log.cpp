
// Author: Pierce Brooks

#include "Log.hpp"

sfml::Log* sfml::Log::self = nullptr;

sfml::Log::Log() :
    ss(new std::stringstream())
{
    self = this;
}

sfml::Log::~Log()
{
    delete ss;
}

sfml::Log* sfml::Log::getSelf()
{
    if (self == nullptr)
    {
        new Log();
    }
    return self;
}
