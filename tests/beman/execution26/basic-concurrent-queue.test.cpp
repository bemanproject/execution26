// tests/beman/execution26/basic-concurrent-queue.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/basic_concurrent_queue.hpp>
#include <beman/execution26/detail/immovable.hpp>
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
};
} // namespace

TEST(basic_concurrent_queue) {
    static_assert(test_detail::basic_concurrent_queue<int, queue<>>);
    static_assert(not test_detail::basic_concurrent_queue<test_detail::immovable, queue<test_detail::immovable>>);
    static_assert(
        not test_detail::basic_concurrent_queue<const test_detail::immovable&, queue<test_detail::immovable>>);
}