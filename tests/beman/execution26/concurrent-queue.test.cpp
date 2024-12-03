// tests/beman/execution26/concurrent-queue.test.cpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/concurrent_queue.hpp>
#include <beman/execution26/detail/basic_concurrent_queue.hpp>
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
    auto try_push(auto&&, std::error_code&) noexcept -> bool;
    auto try_pop(std::error_code&) noexcept -> std::optional<T>;
};

template <typename Q>
    requires test_detail::basic_concurrent_queue<typename Q::value_type, Q>
auto overload(Q&) {}
template <typename Q>
    requires test_detail::concurrent_queue<typename Q::value_type, Q>
auto overload(Q&) {}
} // namespace

TEST(concurrent_queue) {
    static_assert(test_detail::basic_concurrent_queue<int, queue<>>);
    static_assert(test_detail::concurrent_queue<int, queue<>>);

    queue<int> q;
    overload(q);
}