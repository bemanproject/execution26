// tests/beman/execution26/async-concurrent-queue.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/async_concurrent_queue.hpp>
#include <beman/execution26/detail/basic_concurrent_queue.hpp>
#include <beman/execution26/detail/immovable.hpp>
#include <beman/execution26/execution.hpp>
#include <test/execution.hpp>
#include <optional>
#include <system_error>

// ----------------------------------------------------------------------------

namespace {
template <typename T = int>
struct queue {
    using value_type = T;

    auto is_closed() const noexcept -> bool;
    auto close() noexcept -> void;
    auto push(auto&&) -> void;
    auto push(auto&&, std::error_code&) noexcept -> bool;
    auto pop() -> T;
    auto pop(std::error_code) noexcept -> std::optional<T>;
    auto async_push(auto&&) noexcept -> decltype(test_std::just());
    auto async_pop() noexcept -> decltype(test_std::just(std::declval<T>()));
};

template <typename Q>
    requires test_detail::basic_concurrent_queue<typename Q::value_type, Q>
auto overload(Q&) {}
template <typename Q>
    requires test_detail::async_concurrent_queue<typename Q::value_type, Q>
auto overload(Q&) {}
} // namespace

TEST(concurrent_queue) {
    static_assert(test_detail::basic_concurrent_queue<int, queue<>>);
    static_assert(test_detail::async_concurrent_queue<int, queue<>>);

    queue<int> q;
    overload(q);
}

// ----------------------------------------------------------------------------