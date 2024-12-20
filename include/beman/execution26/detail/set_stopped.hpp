// include/beman/execution26/detail/set_stopped.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SET_STOPPED
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SET_STOPPED

#include <beman/execution26/detail/common.hpp>
#include <utility>

#include <beman/execution26/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
/*!
 * \brief Type of the customization point object for cancellation completions.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 */
struct set_stopped_t {
    template <typename Receiver>
    auto operator()(Receiver&) const
        -> void = BEMAN_EXECUTION26_DELETE("set_stopped requires the receiver to be passed as non-const rvalue");
    template <typename Receiver>
    auto operator()(const Receiver&&) const
        -> void = BEMAN_EXECUTION26_DELETE("set_stopped requires the receiver to be passed as non-const rvalue");
    template <typename Receiver>
    auto operator()(Receiver&&) const -> void
        requires(not requires(Receiver&& receiver) { ::std::forward<Receiver>(receiver).set_stopped(); })
    = BEMAN_EXECUTION26_DELETE("set_stopped requires a suitable member overload on the receiver");
    template <typename Receiver>
        requires(not noexcept(::std::declval<Receiver>().set_stopped()))
    auto operator()(Receiver&&) const
        -> void = BEMAN_EXECUTION26_DELETE("the call to receiver.set_stopped() has to be noexcept");

    template <typename Receiver>
    auto operator()(Receiver&& receiver) const noexcept -> void {
        ::std::forward<Receiver>(receiver).set_stopped();
    }
};

/*!
 * \brief Customization point object for cancellation completions.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 */
inline constexpr set_stopped_t set_stopped{};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#include <beman/execution26/detail/suppress_pop.hpp>

#endif
