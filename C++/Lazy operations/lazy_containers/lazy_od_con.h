#include <cstdio>
#include "lazy_operations.h"

namespace lazy_containers
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

        void print() const
        {
            for (const auto &val : data)
            {
                if constexpr (std::is_integral_v<T>)
                    std::printf("%d ", val);
                else if constexpr (std::is_floating_point_v<T>)
                    std::printf("%f ", val);
            }
            std::printf("\n");
        }
    };

    template <typename T>
    class lazy_array : public od_base<lazy_array<T>, T>
    {
    public:
        auto operator+(const lazy_array &other) const
        {
            return lazy_operations::lazy_wise_op<lazy_array, std::plus<T>, lazy_array, lazy_array>(
                *this, other, std::plus<T>());
        }

        auto operator-(const lazy_array &other) const
        {
            return lazy_operations::lazy_wise_op<lazy_array, std::minus<T>, lazy_array, lazy_array>(
                *this, other, std::minus<T>());
        }

        auto operator*(const lazy_array &other) const
        {
            return lazy_operations::lazy_wise_op<lazy_array, std::multiplies<T>, lazy_array, lazy_array>(
                *this, other, std::multiplies<T>());
        }
    };

    template <typename T>
    class lazy_vector : public od_base<lazy_vector<T>, T>
    {
    public:
        auto operator+(const lazy_vector &other) const
        {
            return lazy_operations::lazy_wise_op<lazy_vector, std::plus<T>, lazy_vector, lazy_vector>(
                *this, other, std::plus<T>());
        }

        auto operator-(const lazy_vector &other) const
        {
            return lazy_operations::lazy_wise_op<lazy_vector, std::minus<T>, lazy_vector, lazy_vector>(
                *this, other, std::minus<T>());
        }

        auto operator*(const lazy_vector &other) const
        {
            return lazy_operations::lazy_dot<T, lazy_vector, lazy_vector>(*this, other);
        }
    };
}