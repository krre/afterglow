#pragma once
#include <coroutine>

struct CoAwaiter {
    CoAwaiter(std::coroutine_handle<> handle) : m_handle(handle) {}

    CoAwaiter() {}

    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> handle) {
        m_handle = handle;
    }

    void await_resume() {}

    void resume() {
        if (m_handle && !m_handle.done()) {
            m_handle.resume();
        }
    }

private:
    std::coroutine_handle<> m_handle;
};

struct CoTask {
    struct promise_type;
    using CoHandle = std::coroutine_handle<promise_type>;

    struct promise_type {
        auto get_return_object() { return CoHandle::from_promise(*this); }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };

    struct TaskCoAwaiter {
        TaskCoAwaiter(CoHandle handle) : m_handle(handle)  { }

        TaskCoAwaiter() {}

        bool await_ready() { return false; }
        void await_suspend(CoHandle handle [[maybe_unused]]) {}

        void await_resume() {
            if (m_handle && !m_handle.done()) {
                m_handle.resume();
            }
        }

        CoHandle m_handle;
    };

    CoTask(CoHandle handle) : m_handle(handle) {}

    ~CoTask() {
        if (m_handle.done()) {
            m_handle.destroy();
        }
    }

    TaskCoAwaiter operator co_await() {
        return TaskCoAwaiter(m_handle);
    }

private:
    CoHandle m_handle;
};
