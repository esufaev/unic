#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <math.h>

#include "thread_pool_executor.h"
#include "parfor.h"

pot::coroutines::task<int> async_func_1(int x)
{
    // printf("Async func 1: %llu\n", std::this_thread::get_id());
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // printf("RES 1 IS READY: %d\n", 42 * x);

    co_return 42 * x;
}

pot::coroutines::task<int> async_func_2(int x)
{
    // printf("Async func 2: %llu\n", std::this_thread::get_id());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto res = co_await async_func_1(x);
    // printf("RES 2 IS READY\n");

    co_return res * 2;
}

pot::coroutines::task<void> nested_loops_test()
{
    std::vector<pot::coroutines::task<void>> tasks; // Убран фиксированный размер
    tasks.reserve(3);
    pot::executors::thread_pool_executor_gq executor("Main", 10);

    for (int i = 0; i < 100; ++i)
    {
        auto task = [i]() -> pot::coroutines::task<void> // Захват i по значению
        {
            std::cout << "Outer loop iteration: " << i << std::endl;
            for (int j = 0; j < 100; ++j)
            {
                // std::cout << "  Inner loop iteration: " << j << std::endl;
                auto result = co_await async_func_1(i + j); // Добавлено ожидание результата
            }
            co_return;
        };
        tasks.push_back(executor.run(task));
    }

    // std::this_thread::sleep_for(std::chrono::seconds(5));

    for (auto &task : tasks)
    {
        co_await task;
    }
    co_return;
}

int main()
{
    constexpr auto vec_size = 100;

    std::vector<double> vec_a(vec_size);
    std::vector<double> vec_b(vec_size);
    std::vector<double> vec_c(vec_size);

    std::ranges::fill(vec_a, 1.0);
    std::ranges::fill(vec_b, 2.0);
    auto clear_c = [&vec_c]
    { std::ranges::fill(vec_c, 0.0); };

    clear_c();

    auto pool = pot::executors::thread_pool_executor_lq("Main");

    auto future = pot::algorithms::parfor(pool, 0, vec_size, [&](int i) -> pot::coroutines::task<void>
                                          {
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));

        printf("Task %d\n", i);
        // std::cout << std::this_thread::get_id() << std::endl;
        vec_c[i] = vec_a[i] + vec_b[i];
        co_return; });
    printf("Future is ready\n");
    future.get();

    // std::this_thread::sleep_for(std::chrono::seconds(4));

    printf("Right: %b\n", std::all_of(vec_c.begin(), vec_c.end(), [](const auto &v)
                                      { return v == 3.0; }));

    return 0;
}