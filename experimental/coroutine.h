#include <atomic>
#include <coroutine>
#include <exception>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <cassert>
#include <utility>
#include <thread>
#include <iostream>

namespace pot::tasks::details
{

    template <typename T>
    class shared_state
    {
    public:
        using value_type = T;
        using variant_type = std::conditional_t<std::is_void_v<T>,
                                                std::variant<std::monostate, std::exception_ptr>,
                                                std::variant<std::monostate, T, std::exception_ptr>>;

        shared_state() = default;

        shared_state(const shared_state &) = delete;
        shared_state &operator=(const shared_state &) = delete;

        template <typename U = T>
        void set_value(U &&value)
            requires(!std::is_void_v<T>)
        {
            if (m_ready.exchange(true, std::memory_order_release))
            {
                throw std::runtime_error("Value already set in shared_state.");
            }
            m_data.template emplace<T>(std::forward<U>(value));
            notify_continuation();
        }

        void set_value()
            requires(std::is_void_v<T>)
        {
            if (m_ready.exchange(true, std::memory_order_release))
            {
                throw std::runtime_error("Value already set in shared_state.");
            }
            m_data = std::monostate{};
            notify_continuation();
        }

        void set_exception(std::exception_ptr ex)
        {
            if (m_ready.exchange(true, std::memory_order_release))
            {
                throw std::runtime_error("Exception already set in shared_state.");
            }
            m_data = ex;
            notify_continuation();
        }

        T get()
        {
            wait();
            if (std::holds_alternative<std::exception_ptr>(m_data))
            {
                std::rethrow_exception(std::get<std::exception_ptr>(m_data));
            }
            if constexpr (!std::is_void_v<T>)
            {
                return std::get<T>(m_data);
            }
        }

        void set_continuation(std::coroutine_handle<> continuation) noexcept
        {
            m_continuation = continuation;
        }

        void wait() const
        {
            while (!m_ready.load(std::memory_order_acquire) || !std::get_if<T>(&m_data))
            {
                std::this_thread::yield();
            }
        }

    private:
        void notify_continuation()
        {
            if (m_continuation)
            {
                m_continuation.resume();
            }
        }

        std::atomic<bool> m_ready{false};
        variant_type m_data{std::monostate{}};
        std::coroutine_handle<> m_continuation{nullptr};
    };

} // namespace pot::tasks::details

namespace pot::coroutines
{

    template <typename T>
    class basic_promise_type
    {
    protected:
        std::shared_ptr<tasks::details::shared_state<T>> m_state;

    public:
        std::coroutine_handle<> m_continuation;
        std::atomic_flag m_is_resuming = ATOMIC_FLAG_INIT;
        basic_promise_type() : m_state(std::make_shared<tasks::details::shared_state<T>>()) {}

        void set_continuation(std::coroutine_handle<> continuation) noexcept
        {
            m_continuation = continuation;
        }

        std::coroutine_handle<> continuation() const noexcept
        {
            return m_continuation;
        }

        auto get_shared_state() const noexcept { return m_state; }

        auto operator co_await() noexcept
        {
            return *this;
        }

        struct awaiter_t
        {
            bool await_ready() const noexcept { return false; }
            std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) noexcept
            {
                return h;
            }
            void await_resume() const noexcept {}
        };

        template <typename ValueType>
            requires(!std::is_void_v<T> && std::is_convertible_v<ValueType, T>)
        void set_value(ValueType &&value)
        {
            assert(m_state);
            m_state->set_value(std::forward<ValueType>(value));
        }

        void set_value()
            requires std::is_void_v<T>
        {
            assert(m_state);
            m_state->set_value();
        }

        void set_exception(std::exception_ptr ex)
        {
            assert(m_state);
            m_state->set_exception(ex);
        }

        static constexpr std::suspend_always initial_suspend() noexcept { return {}; }
        static constexpr std::suspend_always final_suspend() noexcept { return {}; }
    };

    template <typename T>
    class [[nodiscard]] task
    {
    public:
        struct promise_type : public basic_promise_type<T>
        {
            task get_return_object()
            {
                return task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            template <typename U>
                requires std::convertible_to<U, T>
            void return_value(U &&value)
            {
                this->m_state->set_value(std::forward<U>(value));
            }

            void unhandled_exception()
            {
                this->m_state->set_exception(std::current_exception());
            }
        };

        using handle_type = std::coroutine_handle<promise_type>;

        explicit task(handle_type h) noexcept : m_handle(h) {}

        task(task &&rhs) noexcept : m_handle(rhs.m_handle)
        {
            rhs.m_handle = nullptr;
        }

        task &operator=(task &&rhs) noexcept
        {
            if (this != &rhs)
            {
                if (m_handle)
                {
                    m_handle.destroy();
                }
                m_handle = rhs.m_handle;
                rhs.m_handle = nullptr;
            }
            return *this;
        }

        task(const task &) = delete;
        task &operator=(const task &) = delete;

        ~task()
        {
            if (m_handle && !m_handle.promise().m_continuation)
            {
                m_handle.destroy();
            }
        }

        bool await_ready() const noexcept
        {
            return m_handle.done();
        }

        void await_suspend(std::coroutine_handle<> continuation) noexcept
        {
            if (m_handle && !m_handle.done() && !m_handle.promise().m_is_resuming.test_and_set(std::memory_order_acquire))
            {
                m_handle.promise().set_continuation(continuation);
                m_handle.resume();
            }
            m_handle.promise().m_is_resuming.clear(std::memory_order_release);
        }

        T await_resume()
        {
            return m_handle.promise().get_shared_state()->get();
        }

        T get()
        {
            if (m_handle && !m_handle.done() && !m_handle.promise().m_is_resuming.test_and_set(std::memory_order_acquire))
            {
                m_handle.resume();
            }
            m_handle.promise().m_is_resuming.clear(std::memory_order_release);
            return m_handle.promise().get_shared_state()->get();
        }

        class iterator
        {
            handle_type m_handle;

        public:
            using value_type = T;
            using difference_type = std::ptrdiff_t;

            iterator(handle_type h = nullptr) : m_handle(h) {}

            value_type operator*() const { return m_handle.promise().current_value(); }
            iterator &operator++()
            {
                m_handle.resume();
                return *this;
            }
            bool operator==(const iterator &) const = default;
        };

        auto begin() { return iterator(m_handle); }
        auto end() { return iterator(); }

        auto operator co_await() noexcept
        {
            struct awaiter
            {
                handle_type m_handle;

                bool await_ready() const noexcept
                {
                    return !m_handle || m_handle.done();
                }

                std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept
                {
                    m_handle.promise().set_continuation(continuation);
                    return m_handle;
                }

                T await_resume()
                {
                    return m_handle.promise().get_shared_state()->get();
                }
            };
            return awaiter{m_handle};
        }

    private:
        handle_type m_handle;
    };

    template <>
    struct task<void>::promise_type : basic_promise_type<void>
    {
        task<void> get_return_object()
        {
            return task<void>{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        void return_void()
        {
            this->m_state->set_value();
        }

        void unhandled_exception()
        {
            this->m_state->set_exception(std::current_exception());
        }
    };
} // namespace pot::coroutines

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
