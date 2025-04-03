#include <iostream>
#include <vector>
#include <cassert>
#include <cstdio>

#include "operations.h"

namespace containers
{
    template <typename Derived, typename T>
    class od_base
    {
    protected:
        std::vector<T> data;

    public:
        using value_type = T;

        od_base(std::size_t size) : data(size) {}
        od_base(std::initializer_list<T> list) : data(list) {}

        std::size_t size() const { return data.size(); }
        T &operator[](std::size_t i) { return data[i]; }
        const T &operator[](std::size_t i) const { return data[i]; }

        Derived operator+(const Derived &other) const
        {
            return operations::wise_op<Derived, std::plus<T>>::apply(*static_cast<const Derived *>(this), other);
        }

        Derived operator-(const Derived &other) const
        {
            return operations::wise_op<Derived, std::minus<T>>::apply(*static_cast<const Derived *>(this), other);
        }

        void print() const
        {
            for (const auto &val : data)
            {
                if constexpr (std::is_integral_v<T>)
                {
                    std::printf("%d ", val);
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    std::printf("%f ", val);
                }
            }
            std::printf("\n");
        }
    };

    template <typename T>
    class array : public od_base<array<T>, T>
    {
    public:
        array operator*(const array &other) const
        {
            return operations::wise_op<array, std::multiplies<T>>::apply(*this, other);
        }
    };

    template <typename T>
    class vector : public od_base<vector<T>, T>
    {
    public:
        T operator*(const vector &other) const
        {
            return operations::dot_product<vector>::apply(*this, other);
        }
    };
}
