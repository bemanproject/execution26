// include/beman/execution26/detail/stop_callback_for_t.hpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_STOP_CALLBACK_FOR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_STOP_CALLBACK_FOR

#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
template <class Token, class CallbackFun>
using stop_callback_for_t = typename Token::template callback_type<CallbackFun>;
}

namespace beman::execution26::detail {
template <typename CallbackFun, typename Token, typename Initializer = CallbackFun>
concept stoppable_callback_for =
    ::std::invocable<CallbackFun> && ::std::constructible_from<CallbackFun, Initializer> &&
    requires { typename ::beman::execution26::stop_callback_for_t<Token, CallbackFun>; } &&
    ::std::constructible_from<::beman::execution26::stop_callback_for_t<Token, CallbackFun>, Token, Initializer> &&
    ::std::constructible_from<::beman::execution26::stop_callback_for_t<Token, CallbackFun>, Token&, Initializer> &&
    ::std::
        constructible_from<::beman::execution26::stop_callback_for_t<Token, CallbackFun>, const Token&, Initializer>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
