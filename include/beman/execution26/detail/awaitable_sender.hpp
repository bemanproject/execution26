// include/beman/execution26/detail/awaitable_sender.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_AWAITABLE_SENDER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_AWAITABLE_SENDER

#include <beman/execution26/detail/env_of_t.hpp>
#include <beman/execution26/detail/single_sender.hpp>

#include <concepts>
#include <coroutine>

namespace beman::execution26::detail {
template <class Sndr, class Promise>
concept awaitable_sender =
    ::beman::execution26::detail::single_sender<Sndr, ::beman::execution26::env_of_t<Promise>> &&
    requires(Promise& prom) {
        { prom.unhandled_stopped() } -> ::std::convertible_to<::std::coroutine_handle<>>;
    };
} // namespace beman::execution26::detail

#endif
