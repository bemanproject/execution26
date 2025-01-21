// src/beman/execution/tests/meta-contains.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/meta_contains.hpp>
#include <test/execution.hpp>

// ----------------------------------------------------------------------------

TEST(meta_contains) {
    static_assert(not test_detail::meta::contains<bool>);
    static_assert(not test_detail::meta::contains<bool, char>);
    static_assert(not test_detail::meta::contains<bool, char, double>);
    static_assert(not test_detail::meta::contains<bool, char, double, int, long, unsigned>);
    static_assert(not test_detail::meta::contains<bool, int, int, int, int, int>);

    static_assert(test_detail::meta::contains<bool, bool>);
    static_assert(test_detail::meta::contains<bool, char, bool>);
    static_assert(test_detail::meta::contains<bool, char, double, bool>);
    static_assert(test_detail::meta::contains<bool, char, double, int, long, unsigned, bool>);
    static_assert(test_detail::meta::contains<bool, int, int, int, int, int, bool>);

    static_assert(test_detail::meta::contains<bool, bool, wchar_t>);
    static_assert(test_detail::meta::contains<bool, char, bool, wchar_t>);
    static_assert(test_detail::meta::contains<bool, char, double, bool, wchar_t>);
    static_assert(test_detail::meta::contains<bool, char, double, int, long, unsigned, bool, wchar_t>);
    static_assert(test_detail::meta::contains<bool, int, int, int, int, int, bool, wchar_t>);
}
