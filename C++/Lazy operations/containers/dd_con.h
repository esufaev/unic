#include <iostream>
#include <vector>
#include <cassert>
#include <cstdio>

#include "operations.h"

namespace containers
{
    template <typename Derived, typename T>
    class dd_base
    {
    protected:
        size_t m_rows, m_cols;
        std::vector<T> data;

    public:
        using value_type = T;

        dd_base(size_t r, size_t c) : m_rows(r), m_cols(c), data(r * c, static_cast<T>(0)) {}
        dd_base(size_t r, size_t c, std::initializer_list<T> values) : m_rows(r), m_cols(c), data(values)
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

        Derived operator+(const Derived &other) const
        {
            return operations::wise_op2d<Derived, std::plus<T>>::apply(*static_cast<const Derived *>(this), other);
        }

        Derived operator-(const Derived &other) const
        {
            return operations::wise_op2d<Derived, std::minus<T>>::apply(*static_cast<const Derived *>(this), other);
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
    class array2d : public dd_base<array2d<T>, T>
    {
    public:
        array2d operator*(const array2d &other) const
        {
            return operations::wise_op2d<array2d, std::multiplies<T>>(*this, other);
        }
    };

    template <typename T>
    class matrix : public dd_base<matrix<T>, T>
    {
    public:
        matrix operator*(const matrix &other) const
        {
            return operations::matrix_mult<matrix>::apply(*this, other);
        }
    };
}