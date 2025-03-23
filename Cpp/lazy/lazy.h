#pragma once

#include <cstdlib>
#include <cstddef>
#include <utility>
#include <iostream>
#include <chrono>
#include <functional>
#include <cassert>

// #define USING_LAZY_OPERATIONS(CB, DERIVED, STORAGE, MODE) \
//     using CB<DERIVED, STORAGE, MODE>::operator+;          \
//     using CB<DERIVED, STORAGE, MODE>::operator-;          \
//     using CB<DERIVED, STORAGE, MODE>::operator*;

namespace utils
{
    template <typename DurationType, typename IterationCleanUpCallback, typename Func, typename... Args>
    DurationType time_it(size_t n, IterationCleanUpCallback &&callback, Func &&func, Args &&...args)
        requires std::invocable<Func, Args...> &&
                 requires { { std::chrono::duration_cast<DurationType>(std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::now()) }; }
    {
        using clock_type = std::chrono::high_resolution_clock;

        clock_type::duration duration{0};

        for (size_t i = 0; i < n; i++)
        {
            const auto start = clock_type::now();
            std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
            const auto end = clock_type::now();
            duration += end - start;

            std::invoke(std::forward<IterationCleanUpCallback>(callback));
        }

        return std::chrono::duration_cast<DurationType>(duration / n);
    }

    template <typename DurationType, typename Func, typename... Args>
    DurationType time_it(Func &&func, Args &&...args)
        requires std::invocable<Func, Args...> &&
                 requires { { std::chrono::duration_cast<DurationType>(std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::now()) }; }
    {
        return time_it<DurationType, Func, Args...>(1, {}, std::forward<Func>(func), std::forward<Args>(args)...);
    }
}

namespace containers
{
    enum ctype { vec, arr, mat, arr2d };

    enum OperationType { add, sub, mult };

    template <ctype mode>
    concept is_matrix = (mode == ctype::mat) || (mode == ctype::arr2d);

    template <ctype mode>
    concept is_heap = (mode == ctype::mat || mode == ctype::vec);

    template <typename ValueType>
    concept Evaluatable = requires(ValueType value) { value.eval(); };

    template <typename T>
    constexpr auto try_eval(const T& value)
    {
        if constexpr (Evaluatable<T>) { return value.eval(); }
        else { return value; }
    }

    template <typename LHS, typename RHS, typename Derived>
    class operation
    {
    public:
        operation(LHS lhs, RHS rhs, OperationType op) 
        : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)), m_op(op) {}

        template <typename _RHS>
        auto operator+(const _RHS &rhs) { return operation<operation<LHS, RHS, Derived>, _RHS, Derived>(*this, rhs, OperationType::add); }
        template <typename _RHS>
        auto operator-(const _RHS &rhs) { return operation<operation<LHS, RHS, Derived>, _RHS, Derived>(*this, rhs, OperationType::sub); }
        template <typename _RHS>
        auto operator*(const _RHS &rhs) { return operation<operation<LHS, RHS, Derived>, _RHS, Derived>(*this, rhs, OperationType::mult); }

        Derived eval() const;
        operator Derived() const;

    private:
        Derived compute() const;

        LHS m_lhs;
        RHS m_rhs;
        OperationType m_op;
    };

    template <typename T, size_t N>
    struct sdata
    {
    public:
        using value_type = T;
        sdata() { for (size_t i = 0; i < N; i++) m_data[i] = T{ };} 
        sdata(size_t size) { for (size_t i = 0; i < N; i++) m_data[i] = T{ }; }
        T &operator[](size_t i) { return m_data[i]; }
        const T &operator[](size_t i) const { return m_data[i]; }
        size_t size() const { return N; }

    private:
        T m_data[N];
    };

    template <typename T>
    struct hdata
    {
        using value_type = T;
        hdata(size_t size) : m_data(new T[size]), m_size(size) {}
        ~hdata() { delete[] m_data; }
        T &operator[](size_t i) { return m_data[i]; }
        const T &operator[](size_t i) const { return m_data[i]; }
        size_t size() const { return m_size; }

    private:
        T *m_data;
        size_t m_size;
    };

    template <typename Derived, typename storage, ctype mode, bool lazy>
    class container_base
    {
    public:
        using value_type = typename storage::value_type;

        container_base(size_t size) requires(mode == ctype::vec || mode == ctype::arr) : m_storage(size) {}
        container_base(size_t rows, size_t cols) requires(mode == ctype::mat) : m_storage(rows * cols), m_rows(rows), m_cols(cols) {}
        container_base(size_t rows, size_t cols) requires(mode == ctype::arr2d) : m_rows(rows), m_cols(cols) {}

        value_type& operator[](size_t i) { return m_storage[i]; }
        const value_type& operator[](size_t i) const { return m_storage[i]; }

        value_type &operator[](size_t i, size_t j) requires(is_matrix<mode>) { return m_storage[i * m_cols + j]; }
        const value_type &operator[](size_t i, size_t j) const requires(is_matrix<mode>) { return m_storage[i * m_cols + j]; }

        template <typename BinaryOp>
        Derived cwise_op(const Derived &other, BinaryOp op) const
        {
            assert(m_storage.size() == other.size());
            Derived result = *static_cast<const Derived *>(this);
            for (size_t i = 0; i < result.size(); ++i)
            {
                result[i] = op(result[i], other[i]);
            }
            return result;
        }

        size_t size() const requires(!is_matrix<mode>) { return this->m_storage.size(); }

        template <typename _RHS>
        auto operator+(const _RHS &rhs) const requires(lazy) 
        { return operation<Derived, _RHS, Derived>(static_cast<const Derived&>(*this), rhs, OperationType::add); }

        template <typename _RHS>
        auto operator-(const _RHS &rhs) const requires(lazy)
        { return operation<Derived, _RHS, Derived>(static_cast<const Derived&>(*this), rhs, OperationType::sub); }

        template <typename _RHS>
        auto operator*(const _RHS &rhs) const requires(lazy)
        { return operation<Derived, _RHS, Derived>(static_cast<const Derived&>(*this), rhs, OperationType::mult); }

        auto mult_matrix(Derived& container)
        {
            assert(this->m_cols == container.m_rows);
            Derived result(this->m_rows, container.m_cols);
            for (size_t i = 0; i < this->m_rows; ++i)
            {
                for (size_t j = 0; j < container.m_cols; ++j)
                {
                    for (size_t k = 0; k < this->m_cols; ++k)
                    {
                        result[i][j] += this->m_storage[i][k] * container.m_storage[k][j];
                    }
                }
            }
            return result;
        }

        template <typename _RHS>
        Derived &operator=(const _RHS &other) requires Evaluatable<_RHS>
        {
            *static_cast<Derived *>(this) = other.eval();
            return *static_cast<Derived *>(this);
        }

        template <typename _RHS>
        container_base(const _RHS &rhs) requires(Evaluatable<_RHS> && lazy) 
        {
            Derived res = rhs.eval();
            *static_cast<Derived *>(this) = res; 
        }

    protected:
        storage m_storage;
        size_t m_rows;
        size_t m_cols;
    };

    template <typename T, bool lazy>
    class vector : public container_base<containers::vector<T, lazy>, hdata<T>, ctype::vec, lazy>
    {
    public:
        using container_base<containers::vector<T, lazy>, hdata<T>, ctype::vec, lazy>::operator+;
        using container_base<containers::vector<T, lazy>, hdata<T>, ctype::vec, lazy>::operator-;
        using container_base<containers::vector<T, lazy>, hdata<T>, ctype::vec, lazy>::operator*;

        static constexpr ctype con_type = ctype::vec;

        template <typename... Args>
        vector(size_t size) : container_base<containers::vector<T, lazy>, hdata<T>, ctype::vec, lazy>(size) {}
        size_t size() const { return this->m_storage.size(); }

        vector(const vector &other) : container_base<vector<T, lazy>, hdata<T>, ctype::vec, lazy>(other.size())
        {
            for (size_t i = 0; i < other.size(); ++i) { this->m_storage[i] = other[i]; }
        }

        vector& operator=(const vector &other)
        {
            if (this != &other)
            {
                assert(this->size() == other.size());
                for (size_t i = 0; i < other.size(); ++i) { this->m_storage[i] = other[i]; }
            }
            return *this;
        }

        vector operator+(const vector &other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a + b; } ); }
        vector operator-(const vector &other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a - b; } ); }
        vector operator*(const vector &other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a * b; } ); }
    }; 

    template <typename T, size_t N, bool lazy>
    class array : public container_base<containers::array<T, N, lazy>, sdata<T, N>, ctype::arr, lazy>
    {
    public:
        using container_base<containers::array<T, N, lazy>, sdata<T, N>, ctype::arr, lazy>::operator+;
        using container_base<containers::array<T, N, lazy>, sdata<T, N>, ctype::arr, lazy>::operator-;
        using container_base<containers::array<T, N, lazy>, sdata<T, N>, ctype::arr, lazy>::operator*;
        static constexpr ctype con_type = ctype::arr;

        array() : container_base<array, sdata<T, N>, ctype::arr, lazy>(N) {}
        array(size_t size) : container_base<array, sdata<T, N>, ctype::arr, lazy>(N) {}

        array& operator=(const array &other)
        {
            if (this != &other)
            {
                assert(this->size() == other.size());
                for (size_t i = 0; i < other.size(); ++i) { this->m_storage[i] = other[i]; }
            }
            return *this;
        }

        array(const array &other) : container_base<array<T, N, lazy>, sdata<T, N>, ctype::arr, lazy>(N)
        {
            assert(this->size() == other.size());
            for (size_t i = 0; i < other.size(); ++i) { this->m_storage[i] = other[i]; }
        }

        size_t size() const { return N; }

        array operator+(const array &other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a + b; } ); }
        array operator-(const array &other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a - b; } ); }
        array operator*(const array &other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a * b; } ); }
    };

    template <typename T, bool lazy>
    class matrix : public container_base<containers::matrix<T, lazy>, hdata<T>, ctype::mat, lazy>
    {
    public:
        using container_base<containers::matrix<T, lazy>, hdata<T>, ctype::mat, lazy>::operator+;
        using container_base<containers::matrix<T, lazy>, hdata<T>, ctype::mat, lazy>::operator-;
        using container_base<containers::matrix<T, lazy>, hdata<T>, ctype::mat, lazy>::operator*;

        static constexpr ctype con_type = ctype::mat;

        matrix(size_t rows, size_t cols) : container_base<matrix<T, lazy>, hdata<T>, ctype::mat, lazy>(rows, cols) {} 

        size_t rows() const { return this->m_rows; }
        size_t cols() const { return this->m_cols; }
        size_t size() const { return this->m_storage.size(); }

        matrix(const matrix &other) : container_base<containers::matrix<T, lazy>, hdata<T>, ctype::mat, lazy>(other.m_rows, other.m_cols)
        {
            assert(this->m_storage.size() == other.m_storage.size());
            for (size_t i = 0; i < other.m_storage.size(); ++i) { this->m_storage[i] = other.m_storage[i]; }
        }

        matrix& operator=(const matrix &other)
        {
            if (this != &other)
            {
                assert(this->m_storage.size() == other.m_storage.size());
                this->m_storage = hdata<T>(other.size());
                this->m_rows = other.m_rows;
                this->m_cols = other.m_cols;
                for (size_t i = 0; i < other.m_storage.size(); ++i) { this->m_storage[i] = other.m_storage[i]; }
            }
            return *this;
        }

        matrix operator+(const matrix& other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a + b; } ); }
        matrix operator-(const matrix& other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a - b; } ); }

        matrix operator*(const matrix& other) const requires(!lazy)
        {
            assert(cols() == other.rows());
            matrix result(this->m_rows, other.m_cols);
            for (size_t i = 0; i < this->m_rows; ++i)
            {
                for (size_t j = 0; j < other.m_cols; ++j)
                {
                    result[i, j] = T{};
                    for (size_t k = 0; k < this->m_cols; ++k)
                    {
                        result[i, j] = result[i, j] + this->m_storage[i * this->m_cols + k] * other.m_storage[k * other.m_cols + j];
                    }
                }
            }
            return result;
        }
    };

    template <typename T, size_t R, size_t C, bool lazy> 
    class array2d : public container_base<containers::array2d<T, R, C, lazy>, sdata<T, R * C>, ctype::arr2d, lazy>
    {
    public:
        using container_base<containers::array2d<T, R, C, lazy>, sdata<T, R * C>, ctype::arr2d, lazy>::operator+;
        using container_base<containers::array2d<T, R, C, lazy>, sdata<T, R * C>, ctype::arr2d, lazy>::operator-;
        using container_base<containers::array2d<T, R, C, lazy>, sdata<T, R * C>, ctype::arr2d, lazy>::operator*;
        static constexpr ctype con_type = ctype::arr2d;

        array2d() : container_base<array2d, sdata<T, R * C>, ctype::arr2d, lazy>(R, C) {}

        array2d(const array2d &other) : container_base<array2d, sdata<T, R * C>, ctype::arr2d, lazy>(R, C)
        {
            assert(this->m_storage.size() == other.m_storage.size());
            for (size_t i = 0; i < other.m_storage.size(); ++i) { this->m_storage[i] = other.m_storage[i]; }
        }

        array2d& operator=(const array2d &other)
        {
            if (this != &other)
            {
                assert(this->m_storage.size() == other.m_storage.size());
                this->m_storage = sdata<T, R * C>();
                for (size_t i = 0; i < other.m_storage.size(); ++i) { this->m_storage[i] = other.m_storage[i]; }
            }
            return *this;
        }

        size_t rows() const { return R; }
        size_t cols() const { return C; }
        size_t size() const { return this->m_storage.size(); }

        array2d operator+(const array2d &other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a + b; } ); }
        array2d operator-(const array2d &other) const requires(!lazy) { return this->cwise_op(other, [] (const auto& a, const auto& b) { return a - b; } ); }

        template <size_t other_col>
        array2d<T, R, other_col, lazy> operator*(const array2d<T, C, other_col, lazy> &other) const requires(!lazy) 
        {
            array2d<T, R, other_col, lazy> result;
            for (size_t i = 0; i < this->m_rows; ++i)
            {
                for (size_t j = 0; j < other_col; ++j)
                {
                    result[i, j] = T{};
                    for (size_t k = 0; k < this->m_cols; ++k)
                    {
                        result[i, j] = result[i, j] + this->m_storage[i * this->m_cols + k] * other[k * other_col + j];
                    }
                }
            }
            return result;
        }
    };

    template <typename Derived>
    struct container_traits
    {
        static Derived construct(size_t rows, size_t cols)
        {
            return Derived(rows, cols);
        }
    };

    template <typename T, size_t R, size_t C, bool lazy>
    struct container_traits<array2d<T, R, C, lazy>>
    {
        static array2d<T, R, C, lazy> construct(size_t /*rows*/, size_t /*cols*/)
        {
            return array2d<T, R, C, lazy>();
        }
    };

    template <typename LHS, typename RHS, typename Derived>
    Derived operation<LHS, RHS, Derived>::compute() const
    {
            Derived lhs = try_eval(m_lhs);
            Derived rhs = try_eval(m_rhs);

            if (m_op == OperationType::add) 
                return lhs.cwise_op(rhs, [](const auto& a, const auto& b) { return a + b; });
            else if (m_op == OperationType::sub) 
                return lhs.cwise_op(rhs, [](const auto& a, const auto& b) { return a - b; });
            else 
            {
                if constexpr (!is_matrix<Derived::con_type>) 
                    return lhs.cwise_op(rhs, [](const auto& a, const auto& b) { return a * b; });
                else 
                {
                    assert(lhs.cols() == rhs.rows());
                    Derived result = container_traits<Derived>::construct(lhs.rows(), rhs.cols());
                    for (size_t i = 0; i < lhs.rows(); ++i) 
                    {
                        for (size_t j = 0; j < rhs.cols(); ++j) 
                        {
                            result[i, j] = typename Derived::value_type{};
                            for (size_t k = 0; k < lhs.cols(); ++k) 
                            {
                                result[i, j] += lhs[i, k] * rhs[k, j];
                            }
                        }
                    }
                    return result;
                }
            }
        }

    template <typename LHS, typename RHS, typename Derived>
    [[nodiscard]] Derived operation<LHS, RHS, Derived>::eval() const { return compute(); }

    template <typename LHS, typename RHS, typename Derived>
    operation<LHS, RHS, Derived>::operator Derived() const  { return compute(); }

    template <typename Container>
    void print(const Container &container)
    {
        if constexpr (is_matrix<Container::con_type>)
        {
            for (size_t i = 0; i < container.rows(); ++i)
            {
                for (size_t j = 0; j < container.cols(); ++j)
                {
                    std::cout << container[i, j] << " ";
                }
                std::cout << std::endl;
            }
        }
        else
        {
            for (size_t i = 0; i < container.size(); ++i) std::cout << container[i] << " ";
            std::cout << std::endl;
        }
    }

    template <typename Container, typename T>
    void fill(Container& container, const T& value) 
    {
        for (size_t i = 0; i < container.size(); ++i) container[i] = value;
    }
}

template <typename T> using vector = containers::vector<T, false>;
template <typename T> using lazy_vector = containers::vector<T, true>;
template <typename T, size_t N> using array = containers::array<T, N, false>;
template <typename T, size_t N> using lazy_array = containers::array<T, N, true>;
template <typename T, size_t R, size_t C> using array2d = containers::array2d<T, R, C, false>;
template <typename T, size_t R, size_t C> using lazy_array2d = containers::array2d<T, R, C, true>;
template <typename T> using matrix = containers::matrix<T, false>;
template <typename T> using lazy_matrix = containers::matrix<T, true>;