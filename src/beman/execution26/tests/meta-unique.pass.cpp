// src/beman/execution26/tests/meta_unique.cpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/meta_unique.hpp>
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace
{
    template <typename...> struct type_list {};
}

auto main() -> int
{
    static_assert(std::same_as<
        type_list<>,
        test_detail::meta::unique<type_list>
        >);
    static_assert(std::same_as<
        type_list<bool>,
        test_detail::meta::unique<type_list, bool>
        >);
    static_assert(std::same_as<
        type_list<bool>,
        test_detail::meta::unique<type_list, bool, bool>
        >);
    static_assert(std::same_as<
        type_list<bool>,
        test_detail::meta::unique<type_list, bool, bool, bool>
        >);
    static_assert(std::same_as<
        type_list<bool, char>,
        test_detail::meta::unique<type_list, bool, bool, bool, char>
        >);
    static_assert(std::same_as<
        type_list<bool, char, double>,
        test_detail::meta::unique<type_list, bool, char, double>
        >);
    static_assert(std::same_as<
        type_list<bool, char, double>,
        test_detail::meta::unique<type_list, bool, bool, char, char, double, double>
        >);
    static_assert(std::same_as<
        type_list<double, char, bool>,
        test_detail::meta::unique<type_list, bool, char, double, char, bool>
        >);
    static_assert(std::same_as<
        type_list<double, bool, char>,
        test_detail::meta::unique<type_list, bool, char, double, bool, char>
        >);
}