#pragma once
#include "ModelHD.hh"
#include <random>

template<typename T, size_t D>
class RandomModelHD : public ModelHD<T, D>
{
    public:
    RandomModelHD(std::mt19937_64& gen)
    {
        std::uniform_real_distribution<> dist(-1.0, 1.0);
        for (auto& it : this->buf)
        {
            it = dist(gen) > 0 ? 1 : -1;
        }
    }
};