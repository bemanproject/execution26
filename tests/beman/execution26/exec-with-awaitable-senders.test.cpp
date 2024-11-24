// src/beman/execution26/tests/exec-with-awaitable-senders.test.cpp   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/execution.hpp>

#include <coroutine>
#include <print>

// This examples shows how to await on senders
// but also how to use awaitables with sender algorithms

namespace exec = beman::execution26;

struct promise;

struct awaitable {
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> h) { h.resume(); }
    int  await_resume() { return 1; }
};

struct coroutine : std::coroutine_handle<promise> {
    using promise_type = ::promise;
};

struct promise : exec::with_awaitable_senders<promise> {
    coroutine           get_return_object() { return {coroutine::from_promise(*this)}; }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void                return_void() {}
    void                unhandled_exception() {}
};

coroutine f() {
    auto [just, value] = co_await exec::when_all(exec::just(42), awaitable{});
    std::print("values: ({}, {})\n", just, value);
    co_return;
}

int main() {
    auto work = f();
    work.resume();
}