#include <chrono>
#include <cstdio>
#include <functional>

#include "./containers/od_con.h"
#include "./containers/dd_con.h"

#include "./lazy_containers/lazy_od_con.h"
#include "./lazy_containers/lazy_dd_con.h"

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

int main()
{
    // containers::array<int> a1({1, 2, 3});
    // containers::array<int> a2({4, 5, 6});
    // containers::array<int> a3 = a1 + a2;
    // containers::array<int> a4 = a1 * a2;

    // containers::vector<int> v1({1, 2, 3});
    // containers::vector<int> v2({4, 5, 6});
    // int dot = v1 * v2;

    // std::printf("array sum: ");
    // a3.print();
    // std::printf("array product: ");
    // a4.print();
    // std::printf("Dot product: %d\n", dot);

    // containers::matrix<int> A(2, 2), B(2, 2);
    // A(0, 0) = 1.1;
    // A(0, 1) = 2.2;
    // A(1, 0) = 3.3;
    // A(1, 1) = 4.4;

    // B(0, 0) = 5.5;
    // B(0, 1) = 6.6;
    // B(1, 0) = 7.7;
    // B(1, 1) = 8.8;

    // containers::matrix<int> C = A * B;
    // C.print();

    // containers::array2d<int> D(2, 2), E(2, 2);
    // D(0, 0) = 1;
    // D(0, 1) = 2;
    // D(1, 0) = 3;
    // D(1, 1) = 4;

    // E(0, 0) = 5;
    // E(0, 1) = 6;
    // E(1, 0) = 7;
    // E(1, 1) = 8;

    // containers::array2d<int> F = D + E;
    // F.print();

    // lazy_containers::lazy_array<int> a1({1, 2, 3});
    // a1.print();

    // lazy_containers::lazy_array<int> a2({2, 4, 5});
    // a2.print();

    // auto res = a1 * a2;

    // res.eval().print();

    // lazy_containers::lazy_matrix<int> a1(2, 2, {1, 2, 3, 4});
    // a1.print();

    // lazy_containers::lazy_matrix<int> a2(2, 2, {3, 4, 9, 0});
    // a2.print();

    // auto res = a1 * a2;
    // res.eval().print();

    auto constexpr size = 10000000;

    containers::array<int> a1(size);
    containers::array<int> a2(size);

    lazy_containers::lazy_array<int> la1(size);
    lazy_containers::lazy_array<int> la2(size);

    for (int i = 0; i < size; i++)
    {
        a1[i] = i;
        a2[i] = i;

        la1[i] = i;
        la2[i] = i;
    }

    auto dur = utils::time_it<std::chrono::nanoseconds>(10, [] {}, [&]()
                                                        { volatile auto res = a1 * a2 + a1 - a2 - a2 - a2; })
                   .count();

    auto ldur = utils::time_it<std::chrono::nanoseconds>(10, [] {}, [&]()
                                                         { auto t = la1 * la2 + la1 - la2 - la2 - la2;;
                                                            volatile auto res = t.eval(); })
                    .count();

    std::cout << "Array: " << dur << " ns\n";
    std::cout << "Lazy array: " << ldur << " ns\n";
    std::cout << (double)ldur / dur << "\n";

    return 0;
}
