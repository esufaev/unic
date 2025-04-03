#pragma once

#include <coroutine>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <source_location>
#include <chrono>
#include <type_traits>
#include <exception>

#include "shared_state.h"

namespace pot::coroutines
{
    template <typename T>
    class basic_promise_type
    {
    protected:
        std::shared_ptr<tasks::details::shared_state<T>> m_state;

    public:
        std::coroutine_handle<> m_continuation;
        basic_promise_type() : m_state(std::make_shared<tasks::details::shared_state<T>>()) {}

        auto get_shared_state() const noexcept { return m_state; }

        auto operator co_await() noexcept
        {
            printf("co_await\n");
            return *this;
        }

        // struct awaiter_t
        // {
        //     bool await_ready() const noexcept { return false; }
        //     std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) noexcept
        //     {
        //         return h;
        //     }
        //     void await_resume() const noexcept {}
        // };

        template <typename ValueType>
            requires(!std::is_void_v<T> && std::is_convertible_v<ValueType, T>)
        void set_value(ValueType &&value)
        {
            printf("set_value\n");
            assert(m_state);
            m_state->set_value(std::forward<ValueType>(value));
            if (m_continuation)
                m_continuation.resume();
        }

        void set_value()
            requires std::is_void_v<T>
        {
            printf("set_value\n");
            assert(m_state);
            m_state->set_value();
            if (m_continuation)
                m_continuation.resume();
        }

        void set_exception(std::exception_ptr ex)
        {
            printf("set_exception\n");
            assert(m_state);
            m_state->set_exception(ex);
        }

        static constexpr std::suspend_always initial_suspend() noexcept
        {
            printf("initial_suspend\n");
            return {};
        }
        static constexpr std::suspend_always final_suspend() noexcept
        {
            printf("final_suspend\n");
            return {};
        }
    };

    template <typename T>
    class [[nodiscard]] task
    {
    public:
        struct promise_type : basic_promise_type<T>
        {
            task get_return_object()
            {
                printf("get_return_object\n");
                return task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }
            
        }
    };
}

namespace pot::traits
{
    template <typename T>
    struct task_value_type;

    template <typename ValueType>
    struct task_value_type<coroutines::task<ValueType>>
    {
        using type = ValueType;
    };

    template <typename ValueType>
    using task_value_type_t = typename task_value_type<ValueType>::type;

    template <typename T>
    struct awaitable_value_type
    {
        using type = T;
    };

    template <template <typename> typename Task, typename T>
    struct awaitable_value_type<Task<T>>
    {
        using type = T;
    };

    template <typename T>
    using awaitable_value_type_t = typename awaitable_value_type<T>::type;

    namespace concepts
    {
        template <typename Type>
        concept is_task = requires(Type t) {
            typename task_value_type_t<std::remove_cvref_t<Type>>;
        };
    }
}
