#pragma once
#include "CountMinSketch.hh"
#include "utils/MurmurHash.hh"
#include <random>
#include <vector>

/**
 * @brief Count-min sketch using MurmurHash3
 * @param K key type
 * @param T underlying type for counters
 */
template<typename K, typename T>
class MurmurCountMinSketch : public CountMinSketch<K, T>
{
    protected:
    std::vector<uint32_t> seeds;

    /**
     * @brief Hash function
     * @param key the query key
     * @param hash_idx the index of the hash function to be used
     * @return hashed value
     */
    uint32_t hash(const K& key, size_t hash_idx) const
    {
        uint32_t result;
        MurmurHash3_x86_32(&key, sizeof(K), seeds[hash_idx], &result);
        return result;
    }


    public:
    MurmurCountMinSketch(size_t w, size_t h, std::mt19937_64& gen)
        : CountMinSketch<K, T>(w, h)
    {
        std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
        for (size_t i = 0; i < this->height; ++i)
        {
            seeds.push_back(dist(gen));
        }
    }

    /**
     * @brief Estimates the number of occurence of given key
     * @param key the query key
     * @return the estimated value
     */
    T estimate(const K& key) const
    {
        T min = std::numeric_limits<T>::max();
        for (size_t i = 0; i < this->height; ++i)
        {
            size_t idx = hash(key, i) % this->width;
            if (min > this->array[i][idx])
            {
                min = this->array[i][idx];
            }
        }
        return min;
    }

    /**
     * @brief Inserts the key to the data structure
     * @param key the query key
     */
    void insert(const K& key)
    {
        for(size_t i = 0; i < this->height; ++i)
        {
            size_t idx = hash(key, i) % this->width;
            this->array[i][idx] += 1;
        }
    }

    /**
     * @brief Perfroms conservative insertion
     * @param key the query key
     */
    void conservative_insert(const K& key)
    {
        T new_val = estimate(key) + 1;
        for(size_t i = 0; i < this->height; ++i)
        {
            size_t idx = hash(key, i) % this->width;
            if (this->array[i][idx] < new_val)
            {
                this->array[i][idx] = new_val;
            }
        }
    }
};