
// Author: Pierce Brooks

#ifndef CV_RANDOM_HPP
#define CV_RANDOM_HPP

#include <random>

namespace cv
{
    class Random
    {
        public:
            Random();
            Random(unsigned int seed);
            virtual ~Random();
            void initialize();
            void initialize(unsigned int seed);
            bool getBool();
            bool getBool(float chance);
            int getInt(int minimum, int maximum);
            float getFloat(float minimum, float maximum);
        private:
            std::mt19937 rng;
            std::random_device seedDefault;
            std::uniform_int_distribution<int> distributorInt;
            std::uniform_real_distribution<float> distributorFloat;
    };
}

#endif
