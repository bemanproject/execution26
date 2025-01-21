// include/beman/execution26/detail/basic_state.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_STATE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_STATE

#include <beman/execution26/detail/impls_for.hpp>
#include <beman/execution26/detail/sender_decompose.hpp>
#include <beman/execution26/detail/tag_of_t.hpp>
#include <beman/execution26/detail/state_type.hpp>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Class template used to present the receiver and operation state for library senders.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <typename Sender, typename Receiver>
struct basic_state {
    basic_state(Sender&& sender, Receiver&& rcvr) noexcept(true)
        : receiver(::std::move(rcvr)),
          state(::beman::execution26::detail::impls_for< ::beman::execution26::tag_of_t<Sender> >::get_state(
              ::std::forward<Sender>(sender), this->receiver)) {}

    Receiver                                                   receiver;
    ::beman::execution26::detail::state_type<Sender, Receiver> state;
};
template <typename Sender, typename Receiver>
basic_state(Sender&&, Receiver&&) -> basic_state<Sender&&, Receiver>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
