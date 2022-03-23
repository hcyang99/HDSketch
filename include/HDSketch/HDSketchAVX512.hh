#pragma once
#include "utils/MurmurHash.hh"
#include <limits>
#include <random>
#include <cstdlib>
#include <cstring>
#include <immintrin.h>

template<typename K>
class HDSketchAVX512
{
    public:
    HDSketchAVX512(size_t s, std::mt19937_64& gen) : sz(s)
    {
        buckets = (char*)std::aligned_alloc(64, sz * 64);
        std::memset(buckets, 0, sz * 64);
        std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
        seed_0 = dist(gen);
        seed_1 = dist(gen);
    }

    ~HDSketchAVX512()
    {
        std::free(buckets);
        buckets = nullptr;
    }

    /**
     * @brief Estimates the number of occurence of given key
     * @param key the query key
     * @return the estimated value
     */
    double estimate(const K& key) const 
    {
        size_t idx = hash(key) % this->sz;
        uint32_t h = project(key);

        __m512i bucket_vec = _mm512_load_epi32(buckets + idx * 64);     // load 32x16 vector from buckets
        __m512i query_vec = hash_to_vec(h);
        __m512i prod_vec = _mm512_madd_epi16(bucket_vec, query_vec);    // FMA
        int dot = _mm512_reduce_add_epi32(prod_vec);                    // Reduce
        return (double)dot / 32;
    }

    /**
     * @brief Inserts the key to the data structure
     * @param key the query key
     */
    void insert(const K& key)
    {
        uint32_t idx = hash(key) % sz;
        uint32_t h = project(key);

        __m512i bucket_vec = _mm512_load_epi32(buckets + idx * 64);     // load 32x16 vector from buckets
        __m512i query_vec = hash_to_vec(h);

        bucket_vec = _mm512_add_epi16(bucket_vec, query_vec);
        _mm512_store_epi32(buckets + idx * 64, bucket_vec);
    }

    protected:
    static constexpr uint32_t FULL_MASK_32 = 0xFFFFFFFFU;
    static constexpr uint32_t UPPER_MASK_32 = 0xFFFF0000U;
    static constexpr uint32_t LOWER_MASK_32 = 0x0000FFFFU;

    static constexpr uint16_t SHIFT_MASK[32] __attribute__((__aligned__(64))) = {
        0x1U, 0x2U, 0x4U, 0x8U, 0x10U, 0x20U, 0x40U, 0x80U,
        0x100U, 0x200U, 0x400U, 0x800U, 0x1000U, 0x2000U, 0x4000U, 0x8000U,
        0x1U, 0x2U, 0x4U, 0x8U, 0x10U, 0x20U, 0x40U, 0x80U,
        0x100U, 0x200U, 0x400U, 0x800U, 0x1000U, 0x2000U, 0x4000U, 0x8000U,
    };

    

    const size_t sz;
    char* buckets;
    uint32_t seed_0;
    uint32_t seed_1;

    /**
     * @brief Hash function for bucket mapping
     */
    uint32_t hash(const K& key) const
    {
        uint32_t result;
        MurmurHash3_x86_32(&key, sizeof(K), seed_0, &result);
        return result;
    }

    /**
     * @brief Hash function for HD projection
     */
    uint32_t project(const K& key) const
    {
        // uint32_t result;
        // MurmurHash3_x86_32(&key, sizeof(K), seed_1, &result);
        // return result;
        return key.u32[0];
    }

    /**
     * @brief Convert 32bit into 32 dimension bi-polar vector
     * @param h 32-bit input
     * @return 32x16 bit vector; vec[i] = (h & (1 << i)) ? 1 : -1;
     */
    __m512i hash_to_vec(uint32_t h) const 
    {
        // load the mask
        __m512i shift_mask = _mm512_load_epi32(SHIFT_MASK);

        uint16_t high = h >> 16U;
        uint16_t low = h;

        __m512i vec = _mm512_set1_epi16(low);                   // set all 32 elements to lower 16 bits of hash
        __m512i v_one = _mm512_set1_epi16(1);                   // vector of ones
        __m512i v_minus_one = _mm512_set1_epi16(-1);            // vector of minus ones
        __m512i v_zero = _mm512_set1_epi16(0);                 // vector of zeros
        
        vec = _mm512_mask_set1_epi16(vec, UPPER_MASK_32, high); // set higher 16 elements to higher 16 bits of hash
        vec = _mm512_and_epi32(shift_mask, vec);                // bit-wise and 
        __mmask32 zero_mask = _mm512_cmp_epi16_mask(vec, v_zero, _MM_CMPINT_EQ);
        vec = _mm512_mask_blend_epi16(zero_mask, v_one, v_minus_one);
        return vec;
    }
};