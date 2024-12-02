// include/beman/execution26/detail/async_concurrent_queue.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_ASYNC_CONCURRENT_QUEUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_ASYNC_CONCURRENT_QUEUE

#include <beman/execution26/execution.hpp>
#include <beman/execution26/detail/basic_concurrent_queue.hpp>
#include <beman/execution26/detail/sender_of.hpp>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <typename T, typename Q>
concept async_concurrent_queue = ::beman::execution26::detail::basic_concurrent_queue<T, Q> && requires(Q q, T&& t) {
    { q.async_push(::std::forward<T>(t)) } noexcept -> ::beman::execution26::detail::sender_of<>;
    { q.async_pop() } noexcept -> ::beman::execution26::detail::sender_of<T>;
};
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
