#pragma once
#include <cstdint>

namespace utils
{
    union byte_32
    {
        char c[32];
        uint32_t u32[8];

        byte_32() : u32() {}

        bool operator==(const byte_32& other) const;
    };

    using Compressed128Mer = byte_32;

    /**
     * @brief Load Compressed128Mer from global string
     * @param data global string
     * @param offset
     * @param out the 128Mer
     */
    void Read128Mer(uint32_t* data, uint32_t offset, Compressed128Mer& out);
}

