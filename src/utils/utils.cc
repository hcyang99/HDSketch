#include "utils/utils.hh"

namespace utils
{
    bool byte_32::operator==(const byte_32& other) const
    {
        for (int i = 0; i < 8; ++i)
        {
            if (u32[i] != other.u32[i])
                return false;
        }
        return true;
    }
}

