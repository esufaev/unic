#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <cassert>

#include "parfor.h"
#include "this_thread.h"

pot::coroutines::task<int> async_computation_1(int value)
{
    pot::this_thread::sleep_for(std::chrono::seconds(3));
    int result = value * 5;
    pot::this_thread::sleep_for(std::chrono::seconds(3));
    printf("Coroutine 1 finished with result: %d\n\r", result);
    pot::this_thread::sleep_for(std::chrono::seconds(3));
    co_return result;
}

pot::coroutines::task<int> async_computation_2(int value)
{
    int result = co_await async_computation_1(value);
    pot::this_thread::sleep_for(std::chrono::seconds(3));
    printf("Coroutine 2 finished with result: %d\n\r", result);
    co_return result;
}

int main()
{
    pot::executors::thread_pool_executor_lq executor("main");

    auto future = executor.run(async_computation_2, 5);
    printf("Waiting...\n\r");
    pot::this_thread::sleep_for(std::chrono::seconds(3));

    printf("RESULT: %d\n\r", future.get());

    return 0;
}
