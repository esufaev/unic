#include <immintrin.h>
#include <iostream>

int main()
{
    constexpr auto N = 1000000000;
    float sumTotal = 0.0f;
    __m256 sumVec = _mm256_setzero_ps(); 

    __m256 A256 = _mm256_set1_ps(1.0f);

    for (int i = 0; i < N; i += 8)
    {
        sumVec = _mm256_add_ps(A256, sumVec); 
    }

    alignas(32) float sumArray[8];
    _mm256_store_ps(sumArray, sumVec);

    for (int j = 0; j < 8; j++)
    {
        sumTotal += sumArray[j]; 
    }

    std::cout << "Total sum: " << sumTotal << std::endl;

    return 0;
}
