// include/test/execution.hpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_TEST_EXECUTION
#define INCLUDED_TEST_EXECUTION

// #include <beman/execution26/stop_token.hpp>
#include <concepts>
#include <cstddef>

#undef NDEBUG
#include <cassert>

#define ASSERT(condition) assert(condition)
#define TEST(name) auto main() -> int

namespace beman::execution26 {}

namespace beman::execution26::detail {};

namespace test_std    = ::beman::execution26;
namespace test_stdex           = ::beman::execution26;
namespace test_std_this_thread = ::beman::execution26;
namespace test_detail = ::beman::execution26::detail;

namespace test {
template <typename>
auto type_exists() {}
template <typename T0, typename T1>
auto check_type(T1&&) {
    static_assert(std::same_as<T0, T1>);
}

template <template <typename...> class>
auto use_template() {}
template <typename...>
auto use_type() {}
auto use(auto&&...) noexcept -> void {}

struct throws {
    throws()                                                 = default;
    throws(throws&&) noexcept(false)                         = default;
    throws(const throws&) noexcept(false)                    = default;
    ~throws()                                                = default;
    auto operator=(throws&&) noexcept(false) -> throws&      = default;
    auto operator=(const throws&) noexcept(false) -> throws& = default;
};

} // namespace test

#endif // INCLUDED_TEST_EXECUTION
