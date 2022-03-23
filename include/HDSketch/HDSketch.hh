#pragma once
#include "HV32.hh"
#include "utils/MurmurHash.hh"
#include <random>


/**
 * @brief HDSketch, an approximate hashtable using HD as conflict resolution
 * @param K key type
 * @param V HD vector element type
 */
template<typename K, typename V>
class HDSketch
{
    public:
    using HV = HV32<V>;

    HDSketch(size_t s, std::mt19937_64& gen) : sz(s)
    {
        buckets = new HV[sz]();
        std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
        seed_0 = dist(gen);
        seed_1 = dist(gen);
    }

    ~HDSketch()
    {
        delete[] buckets;
        buckets = nullptr;
    }

    /**
     * @brief Estimates the number of occurence of given key
     * @param key the query key
     * @return the estimated value
     */
    double estimate(const K& key) const 
    {
        uint32_t idx = hash(key) % this->sz;
        uint32_t h = project(key);
        return (double)buckets[idx].dot(HV(h)) / 32;
    }

    /**
     * @brief Inserts the key to the data structure
     * @param key the query key
     */
    void insert(const K& key)
    {
        uint32_t idx = hash(key) % sz;
        uint32_t h = project(key);
        buckets[idx] += HV(h);
    }


    protected:
    const size_t sz;
    HV* buckets;
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
        uint32_t result;
        MurmurHash3_x86_32(&key, sizeof(K), seed_1, &result);
        return result;
        // return key.u32[0];
    }
};