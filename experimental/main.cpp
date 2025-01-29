#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <cassert>

#include "parfor.h"

pot::coroutines::task<int> async_computation_1(int value)
{
    printf("Starting coroutine 1... This thread: %ull\n\r", std::this_thread::get_id());

    std::this_thread::sleep_for(std::chrono::seconds(2));

    int result = value * 2;
    printf("Coroutine 1 finished with result: %d\n\r", result);
    co_return result;
}

pot::coroutines::task<int> async_computation_2(int value)
{
    printf("Starting coroutine 2... This thread: %ull\n\r", std::this_thread::get_id());

    std::this_thread::sleep_for(std::chrono::seconds(2));

    int result = co_await async_computation_1(value);
    printf("Coroutine 2 finished with result: %d\n\r", result);
    co_return result;
}

int main()
{
    printf("Main thread: %ull\n\r", std::this_thread::get_id());

    pot::executors::thread_pool_executor_lq executor("Main");

    auto future = executor.run(async_computation_2, 5);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    printf("Waiting...\n\r");

    printf("RESULT: %d\n\r", future.get());

    return 0;
}
