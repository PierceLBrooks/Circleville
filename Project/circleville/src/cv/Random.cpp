
// Author: Pierce Brooks

#include "Random.hpp"

cv::Random::Random()
{
    initialize();
}

cv::Random::Random(unsigned int seed)
{
    initialize(seed);
}

cv::Random::~Random()
{

}

void cv::Random::initialize()
{
    rng.seed(seedDefault());
}

void cv::Random::initialize(unsigned int seed)
{
    rng.seed(seed);
}

bool cv::Random::getBool()
{
    return (getInt(0,1)==0);
}

bool cv::Random::getBool(float chance)
{
    if ((chance < 0.0f) || (chance > 1.0f))
    {
        return false;
    }
    return (getFloat(0.0f,1.0f)<chance);
}

int cv::Random::getInt(int minimum, int maximum)
{
    distributorInt = std::uniform_int_distribution<int>(minimum, maximum);
    return distributorInt(rng);
}

float cv::Random::getFloat(float minimum, float maximum)
{
    distributorFloat = std::uniform_real_distribution<float>(minimum, maximum);
    return distributorFloat(rng);
}
