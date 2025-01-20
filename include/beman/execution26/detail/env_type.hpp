// include/beman/execution26/detail/env_type.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_ENV_TYPE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_ENV_TYPE

#include <beman/execution26/detail/call_result_t.hpp>
#include <beman/execution26/detail/impls_for.hpp>
#include <beman/execution26/detail/sender_decompose.hpp>
#include <beman/execution26/detail/tag_of_t.hpp>
#include <beman/execution26/detail/state_type.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Helper alias to determine the environment type associated with sender in a basic_sender
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <typename Index, typename Sender, typename Receiver>
using env_type = ::beman::execution26::detail::call_result_t<
    decltype(::beman::execution26::detail::impls_for< ::beman::execution26::tag_of_t<Sender> >::get_env),
    Index,
    ::beman::execution26::detail::state_type<Sender, Receiver>&,
    const Receiver&>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
