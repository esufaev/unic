#include <type_traits>
#include <utility>

template <typename ValueType>
concept Evaluatable = requires(ValueType value) {
    value.eval();
};

template <typename Type, typename T>
constexpr Type try_eval(const T &value)
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

enum OperationType
{
    Add,
    Subtract, 
    Mult
};

template <typename LHS, typename RHS, OperationType OP>
class operation
{
private:
    const LHS           &m_lhs; 
    const RHS           &m_rhs;
          OperationType  m_op;

public:
    operation(const LHS &lhs, const RHS &rhs, OperationType op) : m_lhs(lhs), m_rhs(rhs), m_op(op) {}

    template <typename _RHS>
    auto operator+(const _RHS &rhs) const
    {
        return operation<operation<LHS, RHS, OP>, _RHS, OperationType::Add>(*this, rhs, OperationType::Add);
    }

    template <typename _RHS>
    auto operator-(const _RHS &rhs) const
    {
        return operation<operation<LHS, RHS, OP>, _RHS, OperationType::Subtract>(*this, rhs, OperationType::Subtract);
    }

    auto eval() const
    {
        return ;
    }
};