#include <vector>
#include <cstdio>
#include <utility>

#include <type_traits>
#include <utility>
#include <stdexcept>

class lazy_matrix;

template <typename ValueType>
concept Evaluatable = requires(ValueType value) {
    value.eval();
};

enum OperationType
{
    Add,
    Subtract,
    Mult
};

template <typename T>
constexpr auto try_eval(const T &value)
{
    if constexpr (Evaluatable<T>)
    {
        return value.eval();
    }
    else
    {
        return value;
    }
}

template <typename LHS, typename RHS>
class operation
{
private:
    const LHS &m_lhs;
    const RHS &m_rhs;
    OperationType m_op;

public:
    operation(const LHS &lhs, const RHS &rhs, OperationType op) : m_lhs(lhs), m_rhs(rhs), m_op(op) {printf("operation\n");}

    template <typename _RHS>
    auto operator+(const _RHS &rhs) const
    {
        return operation<operation<LHS, RHS>, _RHS>(*this, rhs, OperationType::Add);
    }

    template <typename _RHS>
    auto operator-(const _RHS &rhs) const
    {
        return operation<operation<LHS, RHS>, _RHS>(*this, rhs, OperationType::Subtract);
    }

    template <typename _RHS>
    auto operator*(const _RHS &rhs) const
    {
        return operation<operation<LHS, RHS>, _RHS>(*this, rhs, OperationType::Mult);
    }

    lazy_matrix eval() const;
    operator lazy_matrix() const;
};

class lazy_matrix
{
private:
    std::vector<double> data;
    size_t rows;
    size_t cols;

public:
    lazy_matrix() = default;

    lazy_matrix(size_t r, size_t c) : data(r * c), rows(r), cols(c) {}

    lazy_matrix(const lazy_matrix &other) : data(other.data), rows(other.rows), cols(other.cols) {}

    template <typename _RHS>
    lazy_matrix(const _RHS &rhs)
        requires Evaluatable<_RHS>
    {
        lazy_matrix res = rhs.eval();
        data = res.data();
        rows = res.get_rows();
        cols = res.get_cols();
    }

    // lazy_matrix(lazy_matrix &&other) noexcept : data(std::move(other.data)), rows(other.rows), cols(other.cols)
    // {
    //     other.rows = 0;
    //     other.cols = 0;
    // }

    lazy_matrix &operator=(const lazy_matrix &other)
    {
        // if (this != &other)
        // {
        //     data = other.data;
        //     rows = other.rows;
        //     cols = other.cols;
        // }
        data = other.data;
        rows = other.rows;
        cols = other.cols;
        return *this;
    }

    template <typename _RHS>
    lazy_matrix &operator=(const _RHS &other)
    {
        *this = other.eval();
        return *this;
    }

    // lazy_matrix &operator=(lazy_matrix &&other) noexcept
    // {
    //     if (this != &other)
    //     {
    //         data = std::move(other.data);
    //         rows = other.rows;
    //         cols = other.cols;
    //         other.rows = 0;
    //         other.cols = 0;
    //     }
    //     return *this;
    // }

    template <typename _RHS>
    auto operator+(const _RHS &rhs)
    {
        return operation(*this, rhs, OperationType::Add);
    }

    template <typename _RHS>
    auto operator-(const _RHS &rhs)
    {
        return operation(*this, rhs, OperationType::Subtract);
    }

    template <typename _RHS>
    auto operator*(const _RHS &rhs)
    {
        return operation(*this, rhs, OperationType::Mult);
    }

    void print() const
    {
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                printf("%8.2f", data[i * cols + j]);
            }
            printf("\n");
        }
    }

    // lazy_matrix eval() const { return *this; }

    size_t get_rows() const { return rows; }
    size_t get_cols() const { return cols; }

    double &operator()(size_t i, size_t j) { return data[i * cols + j]; }
    const double &operator()(size_t i, size_t j) const { return data[i * cols + j]; }
};

template <typename LHS, typename RHS>
[[nodiscard]] lazy_matrix operation<LHS, RHS>::eval() const
{
    if (m_op == OperationType::Add)
        return try_eval(m_lhs) + try_eval(m_rhs);
    else if (m_op == OperationType::Subtract)
        return try_eval(m_lhs) - try_eval(m_rhs);
    // else 
    //     return try_eval(m_lhs) * try_eval(m_rhs);
}

template <typename LHS, typename RHS>
operation<LHS, RHS>::operator lazy_matrix() const
{
    lazy_matrix lhs = try_eval(m_lhs);
    lazy_matrix rhs = try_eval(m_rhs); 

    if (m_op == OperationType::Add || m_op == OperationType::Subtract)
    {
        if (lhs.get_rows() != rhs.get_rows() || lhs.get_cols() != rhs.get_cols())
        {
            throw std::invalid_argument("Matrix dimensions must match for addition or subtraction.");
        }

        lazy_matrix result(lhs.get_rows(), lhs.get_cols());

        for (size_t i = 0; i < lhs.get_rows(); ++i)
        {
            for (size_t j = 0; j < lhs.get_cols(); ++j)
            {
                if (m_op == OperationType::Add)
                {
                    result(i, j) = lhs(i, j) + rhs(i, j);
                }
                else
                {
                    result(i, j) = lhs(i, j) - rhs(i, j);
                }
            }
        }
        return result;
    }
    else if (m_op == OperationType::Mult)
    {
        if (lhs.get_cols() != rhs.get_rows())
        {
            throw std::invalid_argument("Matrix dimensions are not compatible for multiplication.");
        }

        lazy_matrix result(lhs.get_rows(), rhs.get_cols());

        for (size_t i = 0; i < lhs.get_rows(); ++i)
        {
            for (size_t j = 0; j < rhs.get_cols(); ++j)
            {
                double sum = 0.0;
                for (size_t k = 0; k < lhs.get_cols(); ++k)
                {
                    sum += lhs(i, k) * rhs(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }
    else
    {
        throw std::invalid_argument("Unsupported operation.");
    }
}