#pragma once

#include <cassert>
#include <stddef.h>

namespace operations
{
    template <typename Container, typename Op>
    class wise_op
    {
    public:
        static Container apply(const Container &a, const Container &b)
        {
            assert(a.size() == b.size());

            Container result(a.size());
            for (size_t i = 0; i < a.size(); ++i)
            {
                result[i] = Op()(a[i], b[i]);
            }
            return result;
        }
    };

    template <typename Container, typename Op>
    class wise_op2d
    {
    public:
        static Container apply(const Container &a, const Container &b)
        {
            assert(a.rows() == b.rows() && a.cols() == b.cols());

            Container result(a.rows(), a.cols());
            for (size_t i = 0; i < a.rows(); ++i)
            {
                for (size_t j = 0; j < a.cols(); ++j)
                {
                    result(i, j) = Op()(a(i, j), b(i, j));
                }
            }
            return result;
        }
    };

    template <typename Container>
    class dot_product
    {
    public:
        static typename Container::value_type apply(const Container &a, const Container &b)
        {
            assert(a.size() == b.size());

            typename Container::value_type result = 0;
            for (size_t i = 0; i < a.size(); ++i)
            {
                result += a[i] * b[i];
            }
            return result;
        }
    };

    template <typename Container>
    class matrix_mult
    {
    public:
        static Container apply(const Container &a, const Container &b)
        {
            assert(a.cols() == b.rows());

            Container result(a.rows(), b.cols());
            for (size_t i = 0; i < a.rows(); ++i)
            {
                for (size_t j = 0; j < b.cols(); ++j)
                {
                    for (size_t k = 0; k < a.cols(); ++k)
                    {
                        result(i, j) += a(i, k) * b(k, j);
                    }
                }
            }
            return result;
        }
    };
}