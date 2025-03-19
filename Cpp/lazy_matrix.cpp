#include <vector>
#include <cstdio>
#include <utility>

#include "./impl/operation.h"

namespace ias
{
    class lazy_matrix
    {
    private:
        std::vector<double> data; 
        size_t rows;          
        size_t cols;             

    public:
        lazy_matrix() : data(), rows(0), cols(0) {}

        lazy_matrix(size_t r, size_t c) : data(r * c), rows(r), cols(c) {}

        lazy_matrix(const lazy_matrix &other) : data(other.data), rows(other.rows), cols(other.cols) {}

        lazy_matrix(lazy_matrix &&other) noexcept : data(std::move(other.data)), rows(other.rows), cols(other.cols)
        {
            other.rows = 0;
            other.cols = 0;
        }

        lazy_matrix &operator=(const lazy_matrix &other)
        {
            if (this != &other)
            {
                data = other.data;
                rows = other.rows;
                cols = other.cols;
            }
            return *this;
        }

        lazy_matrix &operator=(lazy_matrix &&other) noexcept
        {
            if (this != &other)
            {
                data = std::move(other.data);
                rows = other.rows;
                cols = other.cols;
                other.rows = 0;
                other.cols = 0;
            }
            return *this;
        }

        template <typename _RHS>
        auto operator+(const _RHS &rhs)
        {
            return operation(*this, rhs, OperationType::Add);
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

        lazy_matrix eval() const { return *this; }

        double &operator()(size_t i, size_t j) { return data[i * cols + j]; }
        const double &operator()(size_t i, size_t j) const { return data[i * cols + j]; }
    };
}

int main()
{
    ias::lazy_matrix lm1 = ias::lazy_matrix(3, 3);
    lm1(0, 0) = 1.0;

    ias::lazy_matrix lm2 = ias::lazy_matrix(3, 3);
    lm2(0, 0) = 1.0;

    auto a = lm1 + lm2;

    return 0;
}