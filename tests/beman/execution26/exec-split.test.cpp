// src/beman/execution26/tests/split.test.cpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/split.hpp>
#include <beman/execution26/execution.hpp>
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

struct NonCopyable {
    NonCopyable()                                  = default;
    NonCopyable(NonCopyable&&) noexcept            = default;
    NonCopyable& operator=(NonCopyable&&) noexcept = default;
    ~NonCopyable()                                 = default;
    NonCopyable(const NonCopyable&)                = delete;
    NonCopyable& operator=(const NonCopyable&)     = delete;
};

template <class... Args>
struct types;

void test_destroy_unused_split() {
    auto just               = beman::execution26::just(NonCopyable{});
    auto split              = beman::execution26::split(std::move(just));
    using split_sender_type = decltype(split);
    struct empty_env {};
    static_assert(beman::execution26::sender_in<split_sender_type, empty_env>);
}

void test_destroy_two_unused_split() {
    auto just  = beman::execution26::just(NonCopyable{});
    auto split = beman::execution26::split(std::move(just));
    auto copy  = split;
}

using beman::execution26::detail::type_list;
using beman::execution26::detail::meta::combine;

template <class... Args>
using to_set_vaule_t = type_list<beman::execution26::set_value_t(Args...)>;

void test_completion_sigs_and_sync_wait_on_split() {
    auto just          = beman::execution26::just(NonCopyable{});
    auto split         = beman::execution26::split(std::move(just));
    using split_sender = std::decay_t<decltype(split)>;
    struct empty_env {};
    using expected_value_completions = type_list<beman::execution26::set_value_t(const NonCopyable&)>;
    using value_completions = beman::execution26::value_types_of_t<split_sender, empty_env, to_set_vaule_t, combine>;
    static_assert(std::same_as<value_completions, expected_value_completions>);

    auto eat_completion = beman::execution26::then(split, [&](const NonCopyable&) {});
    ASSERT(beman::execution26::sync_wait(eat_completion));
}

void test_two_sync_waits_on_one_split() {
    auto        just    = beman::execution26::just(NonCopyable{});
    auto        split   = beman::execution26::split(std::move(just));
    const void* pointer = nullptr;
    auto        save_pointer =
        beman::execution26::then(split, [&](const NonCopyable& obj) { pointer = static_cast<const void*>(&obj); });
    auto assert_pointer = beman::execution26::then(
        split, [&](const NonCopyable& obj) { ASSERT(pointer == static_cast<const void*>(&obj)); });
    ASSERT(beman::execution26::sync_wait(save_pointer));
    ASSERT(beman::execution26::sync_wait(assert_pointer));
}

TEST(exec_split) {
    test_destroy_unused_split();
    test_destroy_two_unused_split();
    test_completion_sigs_and_sync_wait_on_split();
    test_two_sync_waits_on_one_split();
}
