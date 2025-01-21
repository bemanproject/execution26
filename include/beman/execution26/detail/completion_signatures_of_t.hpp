// include/beman/execution26/detail/completion_signaturess_of_t.hpp   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURES_OF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURES_OF

#include <beman/execution26/detail/call_result_t.hpp>
#include <beman/execution26/detail/empty_env.hpp>
#include <beman/execution26/detail/get_completion_signatures.hpp>
#include <beman/execution26/detail/sender_in.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
/*!
 * \brief Alias to access the completion signatures of a sender
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 */
template <typename Sender, typename Env = ::beman::execution26::empty_env>
    requires ::beman::execution26::sender_in<Sender, Env>
using completion_signatures_of_t =
    ::beman::execution26::detail::call_result_t< ::beman::execution26::get_completion_signatures_t, Sender, Env>;
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
