#pragma once
#include <cstdint>
#include <cstddef>
#include <limits>

/**
 * @brief Abstract base class for Count-min Sketches
 * @param K key type
 * @param T underlying type for counters
 */
template<typename K, typename T>
class CountMinSketch
{
    public:
    /**
     * @brief Estimates the number of occurence of given key
     * @param key the query key
     * @return the estimated value
     */
    T estimate(const K& key) const;

    /**
     * @brief Inserts the key to the data structure
     * @param key the query key
     */
    void insert(const K& key);

    /**
     * @brief Perfroms conservative insertion
     * @param key the query key
     */
    void conservative_insert(const K& key);

    protected:
    size_t width;
    size_t height;
    T** array;

    CountMinSketch(size_t w, size_t h)
        : width(w), height(h)
    {
        array = new T*[height]();
        for (size_t i = 0; i < height; ++i)
        {
            array[i] = new T[width]();
        }
    }

    ~CountMinSketch()
    {
        for(size_t i = 0; i < height; ++i)
        {
            delete[] array[i];
            array[i] = nullptr;
        }
        delete[] array;
        array = nullptr;
    }
};