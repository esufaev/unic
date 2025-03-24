#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <limits>
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

namespace searches
{
    template <typename T>
    int linear_search(const std::vector<T> &vec, const T &value)
    {
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (vec[i] == value) return i;
        }
        return -1;
    }

    template <typename T>
    int binary_search_loop(const std::vector<T> &vec, const T &value)
    {
        size_t left = 0;
        size_t right = vec.size() - 1;
        while (left <= right)
        {
            size_t mid = left + (right - left) / 2;
            if (vec[mid] == value) return mid;
            else if (vec[mid] < value) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }

    template <typename T>
    int binary_search_recursive_(const std::vector<T> &vec, const T &value, size_t left, size_t right)
    {
        if (left > right) return -1;
        size_t mid = left + (right - left) / 2;
        if (vec[mid] == value) return mid;
        else if (vec[mid] < value) return binary_search_recursive_(vec, value, mid + 1, right);
        else return binary_search_recursive_(vec, value, left, mid - 1);
    }

    template <typename T>
    size_t binary_search_recursive(const std::vector<T> &vec, const T &value)
    {
        return binary_search_recursive_(vec, value, 0, vec.size() - 1);
    }
}

int main()
{
    constexpr size_t size = 100000000;
    std::vector<int> vec(size);
    std::iota(vec.begin(), vec.end(), 0);

    int value = vec[size / 2];

    const size_t n = 1000;

    auto time_linear = utils::time_it<std::chrono::nanoseconds>(n, [] {}, searches::linear_search<int>, vec, value);
    printf("Linear search: %lld ns\n", time_linear.count());

    auto time_binary_loop = utils::time_it<std::chrono::nanoseconds>(n, [] {}, searches::binary_search_loop<int>, vec, value);
    printf("Binary search loop: %lld ns\n", time_binary_loop.count());

    auto time_binary_recursive = utils::time_it<std::chrono::nanoseconds>(n, [] {}, searches::binary_search_recursive<int>, vec, value);
    printf("Binary search recursive: %lld ns\n", time_binary_recursive.count());

    auto time_std_binary = utils::time_it<std::chrono::nanoseconds>(n, [] {}, [&]()
                                                                    { std::binary_search(vec.begin(), vec.end(), value); });
    printf("Std binary search: %lld ns\n", time_std_binary.count());

    return 0;

    return 0;
}