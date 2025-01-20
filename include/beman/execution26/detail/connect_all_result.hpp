// include/beman/execution26/detail/connect_all_result.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION26_DETAIL_CONNECT_ALL_RESULT
#define INCLUDED_INCLUDE_BEMAN_EXECUTION26_DETAIL_CONNECT_ALL_RESULT

#include <beman/execution26/detail/basic_state.hpp>
#include <beman/execution26/detail/connect_all.hpp>
#include <beman/execution26/detail/call_result_t.hpp>
#include <beman/execution26/detail/indices_for.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Helper type used to determine the state type when connecting all senders in a basic_sender
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <typename Sender, typename Receiver>
using connect_all_result =
    ::beman::execution26::detail::call_result_t<decltype(::beman::execution26::detail::connect_all),
                                                ::beman::execution26::detail::basic_state<Sender, Receiver>*,
                                                Sender,
                                                ::beman::execution26::detail::indices_for<Sender> >;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
