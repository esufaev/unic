#include <iostream>
#include <vector>
#include <cassert>
#include <cstdio>

#include "lazy_operations.h"

namespace lazy_containers
{
    template <typename Derived, typename T>
    class lazy_dd_base
    {
    protected:
        size_t m_rows, m_cols;
        std::vector<T> data;

    public:
        using value_type = T;

        lazy_dd_base(size_t r, size_t c) : m_rows(r), m_cols(c), data(r * c, static_cast<T>(0)) {}
        lazy_dd_base(size_t r, size_t c, std::initializer_list<T> values) : m_rows(r), m_cols(c), data(values)
        {
            assert(values.size() == r * c);
        }

        T &operator()(size_t r, size_t c)
        {
            return data[r * m_cols + c];
        }

        const T &operator()(size_t r, size_t c) const
        {
            return data[r * m_cols + c];
        }

        void print() const
        {
            for (size_t i = 0; i < m_rows; ++i)
            {
                for (size_t j = 0; j < m_cols; ++j)
                {
                    if constexpr (std::is_same<T, double>::value)
                    {
                        printf("%.2f ", (*this)(i, j));
                    }
                    else if constexpr (std::is_same<T, int>::value)
                    {
                        printf("%d ", (*this)(i, j));
                    }
                }
                printf("\n");
            }
        }

        size_t rows() const { return m_rows; }
        size_t cols() const { return m_cols; }
    };

    template <typename T>
    class lazy_array2d : public lazy_dd_base<lazy_array2d<T>, T>
    {
    public:
        lazy_array2d(const lazy_array2d &other) : lazy_dd_base<lazy_array2d<T>, T>(other) {}

        template <typename Other>
        auto operator+(const Other &other) const
        {
            return lazy_operations::lazy_wise_op2d<lazy_array2d<T>, std::plus<T>, lazy_array2d<T>, Other>(
                *this, other, std::plus<T>());
        }

        template <typename Other>
        auto operator-(const Other &other) const
        {
            return lazy_operations::lazy_wise_op2d<lazy_array2d<T>, std::minus<T>, lazy_array2d<T>, Other>(
                *this, other, std::minus<T>());
        }

        template <typename Other>
        auto operator*(const Other &other) const
        {
            return lazy_operations::lazy_wise_op2d<lazy_array2d<T>, std::multiplies<T>, lazy_array2d<T>, Other>(
                *this, other, std::multiplies<T>());
        }
    };

    template <typename T>
    class lazy_matrix : public lazy_dd_base<lazy_matrix<T>, T>
    {
    public:
        template <typename Other>
        auto operator+(const Other &other) const
        {
            return lazy_operations::lazy_wise_op2d<lazy_matrix<T>, std::plus<T>, lazy_matrix<T>, Other>(
                *this, other, std::plus<T>());
        }

        template <typename Other>
        auto operator-(const Other &other) const
        {
            return lazy_operations::lazy_wise_op2d<lazy_matrix<T>, std::minus<T>, lazy_matrix<T>, Other>(
                *this, other, std::minus<T>());
        }

        template <typename Other>
        auto operator*(const Other &other) const
        {
            return lazy_operations::lazy_matrix_mult<T, lazy_containers::lazy_matrix<T>, lazy_containers::lazy_matrix<T>>(
                *this, other );
        }
    };
}