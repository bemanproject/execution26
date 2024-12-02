// include/beman/execution26/detail/basic_concurrent_queue.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_CONCURRENT_QUEUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_CONCURRENT_QUEUE

#include <concepts>
#include <optional>
#include <system_error>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <typename T, typename Q>
concept basic_concurrent_queue =
    ::std::move_constructible<::std::remove_cvref_t<T>> && ::std::same_as<::std::decay_t<T>, typename Q::value_type> &&
    requires(Q q, const Q qc, T&& t, ::std::error_code ec) {
        { qc.is_closed() } noexcept -> ::std::same_as<bool>;
        { q.close() } noexcept -> ::std::same_as<void>;
        { q.push(std::forward<T>(t)) } -> ::std::same_as<void>;
        { q.push(std::forward<T>(t), ec) } noexcept -> ::std::same_as<bool>;
        { q.pop(ec) } -> ::std::same_as<::std::optional<T>>;
        { q.pop() } -> ::std::same_as<T>;
    };
} // namespace beman::execution26::detail
// ----------------------------------------------------------------------------

#endif
