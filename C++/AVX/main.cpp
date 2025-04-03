#include <vector>
#include <cassert>
#include <immintrin.h>
#include <algorithm>
#include <iostream>

// Базовый класс для векторов с CRTP
template <typename Derived>
class VectorBase
{
public:
    std::vector<int> data;

    VectorBase(size_t size) : data(size) {}

    Derived operator+(const Derived &other) const
    {
        return static_cast<const Derived *>(this)->add(other);
    }

    Derived operator-(const Derived &other) const
    {
        return static_cast<const Derived *>(this)->subtract(other);
    }

    int dot(const Derived &other) const
    {
        return static_cast<const Derived *>(this)->dot_product(other);
    }

    int& operator[](size_t index)
    {
        return data[index];
    }
    const int& operator[](size_t index) const
    {
        return data[index];
    }

    void print() const 
    {
        for (const auto &element : data)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
};

// Обычный вектор
class vector : public VectorBase<vector>
{
public:
    vector(size_t size) : VectorBase(size) {}

    vector add(const vector &other) const
    {
        assert(data.size() == other.data.size());
        vector result(data.size());
        for (size_t i = 0; i < data.size(); ++i)
        {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    vector subtract(const vector &other) const
    {
        assert(data.size() == other.data.size());
        vector result(data.size());
        for (size_t i = 0; i < data.size(); ++i)
        {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }

    int dot_product(const vector &other) const
    {
        assert(data.size() == other.data.size());
        int sum = 0;
        for (size_t i = 0; i < data.size(); ++i)
        {
            sum += data[i] * other.data[i];
        }
        return sum;
    }
};

// Вектор с AVX-оптимизацией
class avx_vector : public VectorBase<avx_vector>
{
public:
    avx_vector(size_t size) : VectorBase(size) {}

    avx_vector add(const avx_vector &other) const
    {
        assert(data.size() == other.data.size());
        avx_vector result(data.size());
        size_t i = 0;
        for (; i + 7 < data.size(); i += 8)
        {
            __m256i a = _mm256_loadu_si256((__m256i *)&data[i]);
            __m256i b = _mm256_loadu_si256((__m256i *)&other.data[i]);
            __m256i c = _mm256_add_epi32(a, b);
            _mm256_storeu_si256((__m256i *)&result.data[i], c);
        }
        for (; i < data.size(); ++i)
        {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    avx_vector subtract(const avx_vector &other) const
    {
        assert(data.size() == other.data.size());
        avx_vector result(data.size());
        size_t i = 0;
        for (; i + 7 < data.size(); i += 8)
        {
            __m256i a = _mm256_loadu_si256((__m256i *)&data[i]);
            __m256i b = _mm256_loadu_si256((__m256i *)&other.data[i]);
            __m256i c = _mm256_sub_epi32(a, b);
            _mm256_storeu_si256((__m256i *)&result.data[i], c);
        }
        for (; i < data.size(); ++i)
        {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }

    int dot_product(const avx_vector &other) const
    {
        assert(data.size() == other.data.size());
        int sum = 0;
        size_t i = 0;
        for (; i + 7 < data.size(); i += 8)
        {
            __m256i a = _mm256_loadu_si256((__m256i *)&data[i]);
            __m256i b = _mm256_loadu_si256((__m256i *)&other.data[i]);
            __m256i prod = _mm256_mullo_epi32(a, b);
            __m128i lo = _mm256_castsi256_si128(prod);
            __m128i hi = _mm256_extracti128_si256(prod, 1);
            __m256i sum128 = _mm256_add_epi32(lo, hi);
            sum128 = _mm_hadd_epi32(sum128, sum128);
            sum128 = _mm_hadd_epi32(sum128, sum128);
            sum += _mm_extract_epi32(sum128, 0);
        }
        for (; i < data.size(); ++i)
        {
            sum += data[i] * other.data[i];
        }
        return sum;
    }
};

// Базовый класс для матриц с CRTP
template <typename Derived>
class MatrixBase
{
public:
    std::vector<int> data;
    size_t rows, cols;

    MatrixBase(size_t r, size_t c) : data(r * c), rows(r), cols(c) {}

    Derived operator+(const Derived &other) const
    {
        return static_cast<const Derived *>(this)->add(other);
    }

    Derived operator-(const Derived &other) const
    {
        return static_cast<const Derived *>(this)->subtract(other);
    }

    Derived operator*(const Derived &other) const
    {
        return static_cast<const Derived *>(this)->multiply(other);
    }
};

// Обычная матрица
class matrix : public MatrixBase<matrix>
{
public:
    matrix(size_t r, size_t c) : MatrixBase(r, c) {}

    matrix add(const matrix &other) const
    {
        assert(rows == other.rows && cols == other.cols);
        matrix result(rows, cols);
        for (size_t i = 0; i < data.size(); ++i)
        {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    matrix subtract(const matrix &other) const
    {
        assert(rows == other.rows && cols == other.cols);
        matrix result(rows, cols);
        for (size_t i = 0; i < data.size(); ++i)
        {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }

    matrix multiply(const matrix &other) const
    {
        assert(cols == other.rows);
        matrix result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < other.cols; ++j)
            {
                int sum = 0;
                for (size_t k = 0; k < cols; ++k)
                {
                    sum += data[i * cols + k] * other.data[k * other.cols + j];
                }
                result.data[i * other.cols + j] = sum;
            }
        }
        return result;
    }
};

// Матрица с AVX-оптимизацией
class avx_matrix : public MatrixBase<avx_matrix>
{
public:
    avx_matrix(size_t r, size_t c) : MatrixBase(r, c) {}

    avx_matrix add(const avx_matrix &other) const
    {
        assert(rows == other.rows && cols == other.cols);
        avx_matrix result(rows, cols);
        size_t i = 0;
        for (; i + 7 < data.size(); i += 8)
        {
            __m256i a = _mm256_loadu_si256((__m256i *)&data[i]);
            __m256i b = _mm256_loadu_si256((__m256i *)&other.data[i]);
            __m256i c = _mm256_add_epi32(a, b);
            _mm256_storeu_si256((__m256i *)&result.data[i], c);
        }
        for (; i < data.size(); ++i)
        {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    avx_matrix subtract(const avx_matrix &other) const
    {
        assert(rows == other.rows && cols == other.cols);
        avx_matrix result(rows, cols);
        size_t i = 0;
        for (; i + 7 < data.size(); i += 8)
        {
            __m256i a = _mm256_loadu_si256((__m256i *)&data[i]);
            __m256i b = _mm256_loadu_si256((__m256i *)&other.data[i]);
            __m256i c = _mm256_sub_epi32(a, b);
            _mm256_storeu_si256((__m256i *)&result.data[i], c);
        }
        for (; i < data.size(); ++i)
        {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }

    avx_matrix multiply(const avx_matrix &other) const
    {
        assert(cols == other.rows);
        avx_matrix result(rows, other.cols);
        std::fill(result.data.begin(), result.data.end(), 0);
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t k = 0; k < cols; ++k)
            {
                __m256i a = _mm256_set1_epi32(data[i * cols + k]);
                size_t j = 0;
                for (; j + 7 < other.cols; j += 8)
                {
                    __m256i b = _mm256_loadu_si256((__m256i *)&other.data[k * other.cols + j]);
                    __m256i prod = _mm256_mullo_epi32(a, b);
                    __m256i c = _mm256_loadu_si256((__m256i *)&result.data[i * other.cols + j]);
                    __m256i sum = _mm256_add_epi32(c, prod);
                    _mm256_storeu_si256((__m256i *)&result.data[i * other.cols + j], sum);
                }
                for (; j < other.cols; ++j)
                {
                    result.data[i * other.cols + j] += data[i * cols + k] * other.data[k * other.cols + j];
                }
            }
        }
        return result;
    }
};

int main()
{
    avx_vector v1(10), v2(10);
    v1[0] = 1; v2[0] = 1;
    v1.add(v2);



    return 0;
}