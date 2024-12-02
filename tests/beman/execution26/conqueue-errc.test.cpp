// tests/beman/execution26/conqueue_errc.test.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/conqueue_errc.hpp>
#include <test/execution.hpp>
#include <concepts>
#include <system_error>
#include <string_view>

// ----------------------------------------------------------------------------

TEST(conqueue_errc) {
    static_assert(std::same_as<test_std::conqueue_errc, decltype(test_std::conqueue_errc::empty)>);
    static_assert(std::same_as<test_std::conqueue_errc, decltype(test_std::conqueue_errc::full)>);
    static_assert(std::same_as<test_std::conqueue_errc, decltype(test_std::conqueue_errc::closed)>);
    static_assert(std::same_as<test_std::conqueue_errc, decltype(test_std::conqueue_errc::busy)>);

    static_assert(test_std::conqueue_errc::empty != test_std::conqueue_errc::full);
    static_assert(test_std::conqueue_errc::empty != test_std::conqueue_errc::closed);
    static_assert(test_std::conqueue_errc::empty != test_std::conqueue_errc::busy);
    static_assert(test_std::conqueue_errc::full != test_std::conqueue_errc::closed);
    static_assert(test_std::conqueue_errc::full != test_std::conqueue_errc::busy);
    static_assert(test_std::conqueue_errc::closed != test_std::conqueue_errc::busy);
    static_assert(std::is_error_code_enum<test_std::conqueue_errc>());

    static_assert(std::same_as<const std::error_category&, decltype(test_std::conqueue_category())>);
    static_assert(noexcept(test_std::conqueue_category()));
    const auto& cat{test_std::conqueue_category()};
    ASSERT(cat.name() == std::string_view("conqueue"));
    ASSERT(cat.message(static_cast<int>(test_std::conqueue_errc::empty)) == "empty");
    ASSERT(cat.message(static_cast<int>(test_std::conqueue_errc::full)) == "full");
    ASSERT(cat.message(static_cast<int>(test_std::conqueue_errc::closed)) == "closed");
    ASSERT(cat.message(static_cast<int>(test_std::conqueue_errc::busy)) == "busy");
    ASSERT(cat.message(17) == "unknown");

    static_assert(std::same_as<std::error_code, decltype(test_std::make_error_code(test_std::conqueue_errc::empty))>);
    static_assert(noexcept(test_std::make_error_code(test_std::conqueue_errc::full)));
    std::error_code ec{test_std::make_error_code(test_std::conqueue_errc::full)};
    ASSERT(ec.category() == test_std::conqueue_category());
    ASSERT(ec.value() == static_cast<int>(test_std::conqueue_errc::full));

    static_assert(
        std::same_as<std::error_condition, decltype(test_std::make_error_condition(test_std::conqueue_errc::empty))>);
    static_assert(noexcept(test_std::make_error_condition(test_std::conqueue_errc::full)));
    std::error_condition econd{test_std::make_error_condition(test_std::conqueue_errc::full)};
    ASSERT(econd.category() == test_std::conqueue_category());
    ASSERT(econd.value() == static_cast<int>(test_std::conqueue_errc::full));
}
