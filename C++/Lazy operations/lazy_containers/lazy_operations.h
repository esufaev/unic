#pragma once

#include <cassert>
#include <stddef.h>
#include <functional>
#include <vector>

namespace lazy_operations
{
    template <typename T>
    auto try_eval(const T &value)
    {
        if constexpr (requires { value.eval(); })
            return value.eval();
        else
            return value;
    }

    template <typename Container, typename Op, typename LHS, typename RHS>
    class lazy_wise_op
    {
    private:
        const LHS m_lhs;
        const RHS m_rhs;
        Op m_op;

    public:
        lazy_wise_op(const LHS &lhs, const RHS &rhs, Op op)
            : m_lhs(lhs), m_rhs(rhs), m_op(op) {}

        Container eval() const
        {
            auto lhs_eval = try_eval(m_lhs);
            auto rhs_eval = try_eval(m_rhs);
            assert(lhs_eval.size() == rhs_eval.size());
            Container result(lhs_eval.size());
            for (size_t i = 0; i < lhs_eval.size(); ++i)
            {
                result[i] = m_op(lhs_eval[i], rhs_eval[i]);
            }
            return result;
        }

        operator Container() const { return eval(); }

        template <typename Other>
        auto operator+(const Other &other) const
        {
            return lazy_wise_op<Container, std::plus<typename Container::value_type>,
                              lazy_wise_op, Other>(*this, other, std::plus<typename Container::value_type>());
        }

        template <typename Other>
        auto operator-(const Other &other) const
        {
            return lazy_wise_op<Container, std::minus<typename Container::value_type>,
                              lazy_wise_op, Other>(*this, other, std::minus<typename Container::value_type>());
        }

        template <typename Other>
        auto operator*(const Other &other) const
        {
            return lazy_wise_op<Container, std::minus<typename Container::value_type>,
                                lazy_wise_op, Other>(*this, other, std::multiplies<typename Container::value_type>());
        }
    };

    template <typename T, typename LHS, typename RHS>
    class lazy_dot
    {
    private:
        const LHS m_lhs;
        const RHS m_rhs;

    public:
        lazy_dot(const LHS &lhs, const RHS &rhs)
            : m_lhs(lhs), m_rhs(rhs) {}

        T eval() const
        {
            auto lhs_eval = try_eval(m_lhs);
            auto rhs_eval = try_eval(m_rhs);
            assert(lhs_eval.size() == rhs_eval.size());
            T result = 0;
            for (size_t i = 0; i < lhs_eval.size(); ++i)
            {
                result += lhs_eval[i] * rhs_eval[i];
            }
            return result;
        }

        operator T() const { return eval(); }
    };

    template <typename Container, typename Op, typename LHS, typename RHS>
    class lazy_wise_op2d
    {
    private:
        const LHS m_lhs;
        const RHS m_rhs;
        Op m_op;

    public:
        lazy_wise_op2d(const LHS &lhs, const RHS &rhs, Op op)
            : m_lhs(lhs), m_rhs(rhs), m_op(op) {}

        Container eval() const
        {
            auto lhs_eval = try_eval(m_lhs);
            auto rhs_eval = try_eval(m_rhs);
            assert(lhs_eval.rows() == rhs_eval.rows() && lhs_eval.cols() == rhs_eval.cols());
            Container result(lhs_eval.rows(), lhs_eval.cols());
            for (size_t i = 0; i < lhs_eval.rows(); ++i)
            {
                for (size_t j = 0; j < lhs_eval.cols(); ++j)
                {
                    result(i, j) = m_op(lhs_eval(i, j), rhs_eval(i, j));
                }
            }
            return result;
        }

        operator Container() const { return eval(); }

        template <typename Other>
        auto operator+(const Other &other) const
        {
            return lazy_wise_op2d<Container, std::plus<typename Container::value_type>,
                                lazy_wise_op2d, Other>(*this, other, std::plus<typename Container::value_type>());
        }

        template <typename Other>
        auto operator-(const Other &other) const
        {
            return lazy_wise_op2d<Container, std::minus<typename Container::value_type>,
                                lazy_wise_op2d, Other>(*this, other, std::minus<typename Container::value_type>());
        }

        template <typename Other>
        auto operator*(const Other &other) const
        {
            return lazy_wise_op2d<Container, std::multiplies<typename Container::value_type>,
                                  lazy_wise_op2d, Other>(*this, other, std::multiplies<typename Container::value_type>());
        }
    };

    template <typename T, typename LHS, typename RHS>
    class lazy_matrix_mult
    {
    private:
        const LHS m_lhs;
        const RHS m_rhs;

    public:
        lazy_matrix_mult(const LHS &lhs, const RHS &rhs)
            : m_lhs(lhs), m_rhs(rhs) {}

        containers::matrix<T> eval() const
        {
            auto lhs_eval = try_eval(m_lhs);
            auto rhs_eval = try_eval(m_rhs);
            assert(lhs_eval.cols() == rhs_eval.rows());
            containers::matrix<T> result(lhs_eval.rows(), rhs_eval.cols());
            for (size_t i = 0; i < lhs_eval.rows(); ++i)
            {
                for (size_t j = 0; j < rhs_eval.cols(); ++j)
                {
                    for (size_t k = 0; k < lhs_eval.cols(); ++k)
                    {
                        result(i, j) += lhs_eval(i, k) * rhs_eval(k, j);
                    }
                }
            }
            return result;
        }

        operator containers::matrix<T>() const { return eval(); }
    };
}