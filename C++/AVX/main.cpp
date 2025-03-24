#include <immintrin.h>
#include <iostream>
#include <chrono>
#include <functional>

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
        return time_it<DurationType>(1, [] {}, std::forward<Func>(func), std::forward<Args>(args)...);
    }
}

int main()
{
    constexpr auto N = 1000000000;

    // **AVX-версия**
    float sumAVX = 0.0f;
    auto avx_func = [&sumAVX, N]()
    {
        sumAVX = 0.0f;
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
            sumAVX += sumArray[j];
        }
    };

    auto avx_time = utils::time_it<std::chrono::milliseconds>(avx_func);

    float sumLoop = 0.0f;
    auto loop_func = [&sumLoop, N]()
    {
        sumLoop = 0.0f;
        for (int i = 0; i < N; i++)
        {
            sumLoop += 1.0f;
        }
    };

    auto loop_time = utils::time_it<std::chrono::milliseconds>(loop_func);

    printf("AVX time: %lld ms\n", avx_time.count());
    printf("AVX sum: %f\n", sumAVX);
    printf("Loop time: %lld ms\n", loop_time.count());
    printf("Loop sum: %f\n", sumLoop);

    return 0;
}