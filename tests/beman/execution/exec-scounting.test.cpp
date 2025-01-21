// src/beman/execution/tests/exec-scounting-ctor.test.cpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/simple_counting_scope.hpp>
#include <beman/execution/execution.hpp>
#include <test/execution.hpp>
#include <concepts>
#include <optional>

// ----------------------------------------------------------------------------

namespace {
struct receiver {
    using receiver_concept = test_std::receiver_t;
    bool& flag;
    auto  set_value() && noexcept -> void { this->flag = true; }
};

auto test_scounting_ctor() -> void {
    static_assert(std::default_initializable<test_std::simple_counting_scope>);
    static_assert(noexcept(test_std::simple_counting_scope{}));
    static_assert(std::destructible<test_std::simple_counting_scope>);
    static_assert(not std::copy_constructible<test_std::simple_counting_scope>);
    static_assert(not std::move_constructible<test_std::simple_counting_scope>);
    static_assert(not std::assignable_from<test_std::simple_counting_scope&, test_std::simple_counting_scope>);
    static_assert(not std::assignable_from<test_std::simple_counting_scope&, const test_std::simple_counting_scope>);

    // a scope is created unused and can be destroyed:
    {
        test_std::simple_counting_scope scope{};
        test::use(scope);
    }
    // a scope can be used and closed and can be destroyed:
    {
        test_std::simple_counting_scope scope{};
        scope.close();
    }
    // a scope can be joined and destroyed:
    {
        test_std::simple_counting_scope scope{};
        test_std::sync_wait(scope.join());
    }
    // a scope can be closed, joined, and destroyed:
    {
        test_std::simple_counting_scope scope{};
        scope.close();
        test_std::sync_wait(scope.join());
    }
    // a scope with an outstanding token can be destroyed:
    {
        std::optional<test_std::simple_counting_scope> scope_opt;
        scope_opt.emplace();
        auto token{scope_opt->get_token()};
        scope_opt.reset();
        test::use(token);
    }
}

auto test_scounting_assoc_ctor() -> void {
    static_assert(noexcept(test_std::simple_counting_scope::assoc{}));
    ASSERT(test_std::simple_counting_scope::assoc{} ? false : true);
}

auto test_scounting_associate() -> void {
    test_std::simple_counting_scope scope;
    std::optional                   assoc(scope.get_token().try_associate());
    ASSERT(*assoc ? true : false);
    ASSERT(scope.get_token().try_associate() ? true : false);
    std::optional assoc2(scope.get_token().try_associate());
    scope.close();
    ASSERT(scope.get_token().try_associate() ? false : true);

    bool flag{};
    auto op{test_std::connect(scope.join(), receiver{flag})};
    ASSERT(flag == false);
    test_std::start(op);

    ASSERT(flag == false);
    assoc.reset();

    std::optional assoc3{test_std::simple_counting_scope::assoc()};
    *assoc3 = ::std::move(*assoc2);

    ASSERT(flag == false);
    assoc2.reset();

    ASSERT(flag == false);
    assoc3.reset();
    ASSERT(flag == true);
}
} // namespace

// ----------------------------------------------------------------------------

TEST(exec_scounting_ctor) {
    test_scounting_ctor();
    test_scounting_assoc_ctor();
    test_scounting_associate();
}
