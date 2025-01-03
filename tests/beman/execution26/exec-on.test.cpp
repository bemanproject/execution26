// tests/beman/execution26/exec-on.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/on.hpp>
#include <beman/execution26/detail/just.hpp>
#include <beman/execution26/detail/sender_adaptor_closure.hpp>
#include <beman/execution26/detail/then.hpp>
#include <test/execution.hpp>
#include <test/thread_pool.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace {
struct both : test_std::sender_adaptor_closure<both> {
    using sender_concept = test_std::sender_t;
};

static_assert(test_std::sender<both>);
static_assert(test_detail::is_sender_adaptor_closure<both>);

template <test_std::scheduler                    Sch,
          test_std::sender                       Sndr,
          test_detail::is_sender_adaptor_closure Closure,
          typename Both>
auto test_interface(Sch sch, Sndr sndr, Closure closure, Both both) -> void {
    static_assert(requires {
        { test_std::on(sch, sndr) } -> test_std::sender;
    });
    static_assert(not requires { test_std::on(sch, both); });
    static_assert(requires {
        { test_std::on(sndr, sch, closure) } -> test_std::sender;
    });
    static_assert(not requires { test_std::on(both, sch, closure); });

    auto sndr1{test_std::on(sch, sndr)};
    auto sndr2{test_std::on(sndr, sch, closure)};
    test::use(sndr1, sndr2);
}

template <test_detail::sender_for<test_std::on_t> OutSndr>
auto test_transform_env(OutSndr out_sndr) -> void {
    auto e{test_std::on.transform_env(out_sndr, test_std::empty_env{})};
}
} // namespace

TEST(exec_on) {
    test::thread_pool pool{};

    static_assert(std::same_as<const test_std::on_t, decltype(test_std::on)>);
    static_assert(test_detail::is_sender_adaptor_closure<decltype(test_std::then([] {}))>);
    static_assert(not test_detail::is_sender_adaptor_closure<decltype(test_std::just([] {}))>);
    test_interface(pool.get_scheduler(), test_std::just(), test_std::then([] {}), both{});

    test_transform_env(test_std::on(pool.get_scheduler(), test_std::just()));
    test_transform_env(test_std::on(test_std::just(), pool.get_scheduler(), test_std::then([] {})));
}