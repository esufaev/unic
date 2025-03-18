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
    operation(const LHS &lhs, const RHS &rhs, OperationType op) : m_lhs(lhs), m_rhs(rhs), m_op(op) {}

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

    [[nodiscard]] lazy_matrix eval() const
    {
        switch (m_op)
        {
        case OperationType::Add:
            return try_eval(m_lhs) + try_eval(m_rhs);
        case OperationType::Subtract:
            return try_eval(m_lhs) - try_eval(m_rhs);
        case OperationType::Mult:
            return try_eval(m_lhs) * try_eval(m_rhs);
        }
        throw std::runtime_error("Invalid operation type");
        return lazy_matrix();
    }
};