#pragma once
#include "CountMinSketch.hh"
#include <random>
#include <array>

/**
 * @brief Count-min sketch using modulo of LONG_PRIME as hash
 * @param K key type
 * @param T counter type
 */
template<typename K, typename T>
class ModuloCountMinSketch : public CountMinSketch<K, T>
{
    protected:
    const int64_t LONG_PRIME = 4294967311L;
    std::array<uint32_t, 2>* hashes;

    uint32_t get_key_signature(const K& key) const
    {
        const char* key_ptr = (const char*) &key;
        uint32_t sig = 5381;
        for (size_t i = 0; i < sizeof(K); ++i)
        {
            sig = ((sig << 5U) + sig) + key_ptr[i];
        }
        return sig;
    }

    /**
     * @brief Hash function
     * @param sig the signature of the key
     * @param hash_idx the index of the hash function to be used
     * @return hashed value
     */
    size_t hash(uint32_t sig, size_t hash_idx) const
    {
        return (size_t)hashes[hash_idx][0] * sig + hashes[hash_idx][1] % LONG_PRIME;
    }


    public:
    ModuloCountMinSketch(size_t w, size_t h, std::mt19937_64& gen)
        : CountMinSketch<K, T>(w, h), hashes()
    {
        hashes = new std::array<uint32_t, 2>[h];

        constexpr uint32_t rand_max = std::numeric_limits<uint32_t>::max();
        std::uniform_int_distribution<uint32_t> dist(0, rand_max);

        for (size_t i = 0; i < h; ++i)
        {
            hashes[i][0] = uint32_t(double(dist(gen)) * LONG_PRIME / rand_max + 1);
            hashes[i][1] = uint32_t(double(dist(gen)) * LONG_PRIME / rand_max + 1);
        }
    }

    ~ModuloCountMinSketch()
    {
        delete[] hashes;
        hashes = nullptr;
    }

    /**
     * @brief Estimates the number of occurence of given key
     * @param key the query key
     * @return the estimated value
     */
    T estimate(const K& key) const
    {
        auto sig = get_key_signature(key);
        T min = std::numeric_limits<T>::max();
        for (size_t i = 0; i < this->height; ++i)
        {
            size_t idx = hash(sig, i) % this->width;
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
        auto sig = get_key_signature(key);
        for(size_t i = 0; i < this->height; ++i)
        {
            size_t idx = hash(sig, i) % this->width;
            this->array[i][idx] += 1;
        }
    }

    /**
     * @brief Perfroms conservative insertion
     * @param key the query key
     */
    void conservative_insert(const K& key)
    {
        auto sig = get_key_signature(key);
        T min = std::numeric_limits<T>::max();
        for (size_t i = 0; i < this->height; ++i)
        {
            size_t idx = hash(sig, i) % this->width;
            if (min > this->array[i][idx])
            {
                min = this->array[i][idx];
            }
        }
        
        T new_val = min + 1;

        for(size_t i = 0; i < this->height; ++i)
        {
            size_t idx = hash(sig, i) % this->width;
            if (this->array[i][idx] < new_val)
            {
                this->array[i][idx] = new_val;
            }
        }
    }
};