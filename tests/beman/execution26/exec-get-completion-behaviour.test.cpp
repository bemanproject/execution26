// src/beman/execution26/tests/exec-get-completion-behaviour.test.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/get_completion_behaviour.hpp>
#include <beman/execution26/detail/just.hpp>

#include <test/execution.hpp>

#include <type_traits>

namespace {

template <auto Value>
using value = std::integral_constant<decltype(Value), Value>;

struct awaitable {
    static constexpr auto await_ready() noexcept -> bool { return true; }
    auto                  await_suspend(auto parent) noexcept { return parent; }
    auto                  await_resume() noexcept -> void {}
};

void test_constexpr_awaitable() {
    auto completion_behaviour = value<test_std::get_completion_behaviour(awaitable{})>{};
    ASSERT(test_std::completion_behaviour::inline_completion == completion_behaviour);
}
struct sender {
    using completion_behaviour = value<test_std::completion_behaviour::inline_completion>;
};

struct sender2 {
    static constexpr auto get_completion_behaviour() noexcept -> test_std::completion_behaviour {
        return test_std::completion_behaviour::inline_completion;
    }
};

void test_typedef_sender() {
    auto completion_behaviour =
        std::integral_constant<test_std::completion_behaviour, test_std::get_completion_behaviour(sender{})>{};
    ASSERT(test_std::completion_behaviour::inline_completion == completion_behaviour);
}

void test_constexpr_sender() {
    auto completion_behaviour =
        std::integral_constant<test_std::completion_behaviour, test_std::get_completion_behaviour(sender{})>{};
    ASSERT(test_std::completion_behaviour::inline_completion == completion_behaviour);
}

void test_just() {
    auto just                 = test_std::just();
    auto completion_behaviour = test_std::get_completion_behaviour(just, test_std::empty_env{});
    ASSERT(test_std::completion_behaviour::inline_completion == completion_behaviour);
}

void test_constexpr_just() {
    constexpr auto completion_behaviour = test_std::get_completion_behaviour(test_std::just(), test_std::empty_env{});
    ASSERT(test_std::completion_behaviour::inline_completion == completion_behaviour);
}

} // namespace

TEST(exec_get_completion_behaviour) {
    test_constexpr_awaitable();
    test_typedef_sender();
    test_constexpr_sender();
    test_just();
    test_constexpr_just();
}
