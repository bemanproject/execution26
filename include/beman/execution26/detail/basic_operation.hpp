// include/beman/execution26/detail/basic_operation.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_OPERATION
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_OPERATION

#include <beman/execution26/detail/operation_state.hpp>
#include <beman/execution26/detail/basic_state.hpp>
#include <beman/execution26/detail/state_type.hpp>
#include <beman/execution26/detail/sender_decompose.hpp>
#include <beman/execution26/detail/tag_of_t.hpp>
#include <beman/execution26/detail/impls_for.hpp>
#include <beman/execution26/detail/connect_all_result.hpp>
#include <beman/execution26/detail/valid_specialization.hpp>
#include <beman/execution26/detail/start.hpp>
#include <functional>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Class template use to factor out common operation state needs.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <typename Sender, typename Receiver>
    requires ::beman::execution26::detail::
        //-dk:TODO why is the remove_cvref_t needed...?
    valid_specialization<::beman::execution26::detail::state_type, std::remove_cvref_t<Sender>, Receiver>
struct basic_operation : ::beman::execution26::detail::basic_state<Sender, Receiver> {
    // static_assert(std::same_as<Sender, std::remove_cvref_t<Sender>>);
    friend struct ::beman::execution26::start_t;
    using operation_state_concept = ::beman::execution26::operation_state_t;
    using tag_t                   = ::beman::execution26::tag_of_t<Sender>;

    using inner_ops_t = ::beman::execution26::detail::connect_all_result<Sender, Receiver>;
    inner_ops_t inner_ops;

    basic_operation(Sender&& sender, Receiver&& rcvr) noexcept(
        noexcept(::beman::execution26::detail::basic_state<Sender, Receiver>(::std::forward<Sender>(sender),
                                                                             ::std::move(rcvr))) &&
        noexcept(::beman::execution26::detail::connect_all(this,
                                                           ::std::forward<Sender>(sender),
                                                           ::beman::execution26::detail::indices_for<Sender>())))
        : ::beman::execution26::detail::basic_state<Sender, Receiver>(::std::forward<Sender>(sender),
                                                                      ::std::move(rcvr)),
          // NOLINTBEGIN(bugprone-use-after-move,hicpp-invalid-access-moved)
          //-dk:TODO deal with moving the sender twice
          inner_ops(::beman::execution26::detail::connect_all(
              this, ::std::forward<Sender>(sender), ::beman::execution26::detail::indices_for<Sender>())) {}
    // NOLINTEND(bugprone-use-after-move,hicpp-invalid-access-moved)

  private:
    auto start() & noexcept -> void {
        ::std::invoke(
            [this]<::std::size_t... I>(::std::index_sequence<I...>) {
                ::beman::execution26::detail::impls_for<tag_t>::start(
                    this->state, this->receiver, this->inner_ops.template get<I>()...);
            },
            ::std::make_index_sequence<inner_ops_t::size()>{});
    }
};
template <typename Sender, typename Receiver>
basic_operation(Sender&&, Receiver&&) -> basic_operation<Sender&&, Receiver>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
