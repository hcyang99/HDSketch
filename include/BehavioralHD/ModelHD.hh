#pragma once 
#include <cstddef>
#include <array>
#include <algorithm>

/**
 * @brief A behavioral model for HD
 * @param T the underlying type for each element
 * @param D number of dimensions
 */
template <typename T, size_t D>
class ModelHD
{
    public:
    ModelHD() : buf() {}
    ModelHD(const std::array<T, D>& b) : buf(b) {}

    ModelHD& operator=(const ModelHD& other)
    {
        buf = other.buf;
        return *this;
    }

    ModelHD& operator+=(const ModelHD& other)
    {
        for (size_t i = 0; i < D; ++i)
        {
            buf[i] += other.buf[i];
        }
        return *this;
    }   

    ModelHD& operator-=(const ModelHD& other)
    {
        for (size_t i = 0; i < D; ++i)
        {
            buf[i] -= other.buf[i];
        }
        return *this;
    }

    ModelHD& operator*=(const T& scalar)
    {
        for (size_t i = 0; i < D; ++i)
        {
            buf[i] *= scalar;
        }
        return *this;
    }

    ModelHD operator+(const ModelHD& other) const
    {
        ModelHD result;
        std::transform(buf.cbegin(), buf.cend(), other.buf.cbegin(), result.buf.begin(), T::operator+);
        return result;
    }

    ModelHD operator-(const ModelHD& other) const
    {
        ModelHD result;
        std::transform(buf.cbegin(), buf.cend(), other.buf.cbegin(), result.buf.begin(), T::operator-);
        return result;
    }

    /**
     * @brief bind operation
     */
    ModelHD operator*(const ModelHD& other) const
    {
        ModelHD result;
        std::transform(buf.cbegin(), buf.cend(), other.buf.cbegin(), result.buf.begin(), T::operator*);
        return result;
    }

    double l1norm() const
    {
        double norm = 0;
        for (const auto& it : buf)
        {
            norm += it;
        }
        return norm;
    }

    double l2norm() const
    {
        double norm = 0;
        for (const auto& it : buf)
        {
            norm += (double)it * it;
        }
        return norm;
    }

    /**
     * @brief dot product
     */
    double dot(const ModelHD& other) const
    {
        double result = 0;
        for (size_t i = 0; i < D; ++i)
        {
            result += (double)buf[i] * other.buf[i];
        }
        return result;
    }

    protected:
    std::array<T, D> buf;
};