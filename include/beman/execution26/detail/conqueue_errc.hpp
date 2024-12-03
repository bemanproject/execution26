// include/beman/execution26/detail/conqueue_errc.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONQUEUE_ERRC
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONQUEUE_ERRC

#include <string>
#include <system_error>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
enum class conqueue_errc { empty, full, closed, busy };

inline auto conqueue_category() noexcept -> const ::std::error_category&;
inline auto make_error_code(::beman::execution26::conqueue_errc) noexcept -> ::std::error_code;
inline auto make_error_condition(::beman::execution26::conqueue_errc) noexcept -> ::std::error_condition;
} // namespace beman::execution26

namespace std {
template <>
struct is_error_code_enum<::beman::execution26::conqueue_errc> : ::std::true_type {};
} // namespace std

// ----------------------------------------------------------------------------

inline auto beman::execution26::conqueue_category() noexcept -> const ::std::error_category& {
    struct category : ::std::error_category {
        auto name() const noexcept -> const char* override { return "conqueue"; }
        auto message(int value) const -> ::std::string override {
            switch (value) {
            default:
                return "unknown";
            case static_cast<int>(::beman::execution26::conqueue_errc::empty):
                return "empty";
            case static_cast<int>(::beman::execution26::conqueue_errc::full):
                return "full";
            case static_cast<int>(::beman::execution26::conqueue_errc::closed):
                return "closed";
            case static_cast<int>(::beman::execution26::conqueue_errc::busy):
                return "busy";
            }
        }
    };
    static category rc{};
    return rc;
}

inline auto beman::execution26::make_error_code(conqueue_errc e) noexcept -> ::std::error_code {
    return ::std::error_code(static_cast<int>(e), ::beman::execution26::conqueue_category());
}

inline auto beman::execution26::make_error_condition(conqueue_errc e) noexcept -> ::std::error_condition {
    return ::std::error_condition(static_cast<int>(e), ::beman::execution26::conqueue_category());
}

// ----------------------------------------------------------------------------

#endif
