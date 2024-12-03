// include/beman/execution26/detail/concurrent_queue.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONCURRENT_QUEUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONCURRENT_QUEUE

#include <beman/execution26/detail/basic_concurrent_queue.hpp>
#include <concepts>
#include <optional>
#include <utility>
#include <system_error>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <typename T, typename Q>
concept concurrent_queue =
    ::beman::execution26::detail::basic_concurrent_queue<T, Q> && requires(Q q, T&& t, ::std::error_code ec) {
        { q.try_push(::std::forward<T>(t), ec) } -> ::std::same_as<bool>;
        { q.try_pop(ec) } -> ::std::same_as<::std::optional<T>>;
    };
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
