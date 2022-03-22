#pragma once
#include "BehavioralHD/ModelHD.hh"

template<typename T>
class HV32 : public ModelHD<T, 32>
{   
    public:
    HV32() : ModelHD<T, 32>() {}

    /**
     * @brief Constructs 32-dimensional vector from hash value
     * @param hash the input hash value
     */
    HV32(uint32_t hash) : ModelHD<T, 32>()
    {
        for (size_t i = 0; i < 32; ++i)
        {
            if (hash & (1U << i))
            {
                this->buf[i] = 1;
            }
            else 
            {
                this->buf[i] = -1;
            }
        }
    }
};