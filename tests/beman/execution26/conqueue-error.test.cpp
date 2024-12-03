// tests/beman/execution26/conqueue-error.test.cpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/conqueue_error.hpp>
#include <beman/execution26/detail/conqueue_errc.hpp>
#include <test/execution.hpp>
#include <concepts>
#include <string_view>
#include <type_traits>

// ----------------------------------------------------------------------------

TEST(conqueue_error) {
    static_assert(std::is_base_of_v<std::system_error, test_std::conqueue_error>);
    test_std::conqueue_error e0(test_std::conqueue_errc::empty);
    test_std::conqueue_error e1(test_std::conqueue_errc::full);

    ASSERT(e0.code() == make_error_code(test_std::conqueue_errc::empty));
    ASSERT(std::string_view(e0.what()).find("empty") != std::string_view::npos);
    ASSERT(e1.code() == make_error_code(test_std::conqueue_errc::full));
    ASSERT(std::string_view(e1.what()).find("full") != std::string_view::npos);
}