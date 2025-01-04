// include/beman/execution26/detail/basic_sender.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_SENDER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_SENDER

#include <beman/execution26/detail/basic_operation.hpp>
#include <beman/execution26/detail/completion_signatures_for.hpp>
#include <beman/execution26/detail/decays_to.hpp>
#include <beman/execution26/detail/impls_for.hpp>
#include <beman/execution26/detail/product_type.hpp>
#include <beman/execution26/detail/sender.hpp>
#include <beman/execution26/detail/sender_decompose.hpp>
#include <beman/execution26/detail/connect.hpp>
#include <beman/execution26/detail/get_completion_signatures.hpp>
#include <utility>

#include <beman/execution26/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Class template used to factor out common sender implementation for library senders.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <typename Tag, typename Data, typename... Child>
struct basic_sender : ::beman::execution26::detail::product_type<Tag, Data, Child...> {
    friend struct ::beman::execution26::connect_t;
    friend struct ::beman::execution26::get_completion_signatures_t;
    using sender_concept = ::beman::execution26::sender_t;
    using indices_for    = ::std::index_sequence_for<Child...>;

    auto get_env() const noexcept -> decltype(auto) {
        auto data{::beman::execution26::detail::get_sender_data(*this)};
        return ::std::apply(
            [&data](auto&&... c) { return ::beman::execution26::detail::impls_for<Tag>::get_attrs(data.data, c...); },
            data.children);
    }

    template <typename Receiver>
        requires(not::beman::execution26::receiver<Receiver>)
    auto connect(Receiver receiver) = BEMAN_EXECUTION26_DELETE("the passed receiver doesn't model receiver");

  private:
#if __cpp_explicit_this_parameter < 302110L //-dk:TODO need to figure out how to use explicit this with forwarding
    template <::beman::execution26::receiver Receiver>
    auto connect(Receiver receiver) & noexcept(
        noexcept(::beman::execution26::detail::basic_operation<basic_sender&, Receiver>{*this, ::std::move(receiver)}))
        -> ::beman::execution26::detail::basic_operation<basic_sender&, Receiver> {
        return {*this, ::std::move(receiver)};
    }
    template <::beman::execution26::receiver Receiver>
    auto connect(Receiver receiver) const& noexcept(noexcept(
        ::beman::execution26::detail::basic_operation<const basic_sender&, Receiver>{*this, ::std::move(receiver)}))
        -> ::beman::execution26::detail::basic_operation<const basic_sender&, Receiver> {
        return {*this, ::std::move(receiver)};
    }
    template <::beman::execution26::receiver Receiver>
    auto connect(Receiver receiver) && noexcept(
        noexcept(::beman::execution26::detail::basic_operation<basic_sender, Receiver>{::std::move(*this),
                                                                                       ::std::move(receiver)}))
        -> ::beman::execution26::detail::basic_operation<basic_sender, Receiver> {
        return {::std::move(*this), ::std::move(receiver)};
    }
#else
    template <::beman::execution26::detail::decays_to<basic_sender> Self, ::beman::execution26::receiver Receiver>
    auto
    connect(this Self&& self,
            Receiver receiver) noexcept(noexcept(::beman::execution26::detail::basic_operation<basic_sender, Receiver>{
        ::std::forward<Self>(self), ::std::move(receiver)}))
        -> ::beman::execution26::detail::basic_operation<Self, Receiver> {
        return {::std::forward<Self>(self), ::std::move(receiver)};
    }
#endif
#if __cpp_explicit_this_parameter < 302110L
    template <typename Env>
    auto
    get_completion_signatures(Env&&) && -> ::beman::execution26::detail::completion_signatures_for<basic_sender, Env> {
        return {};
    }
    template <typename Env>
    auto get_completion_signatures(
        Env&&) const&& -> ::beman::execution26::detail::completion_signatures_for<const basic_sender, Env> {
        return {};
    }
    template <typename Env>
    auto
    get_completion_signatures(Env&&) & -> ::beman::execution26::detail::completion_signatures_for<basic_sender, Env> {
        return {};
    }
    template <typename Env>
    auto get_completion_signatures(
        Env&&) const& -> ::beman::execution26::detail::completion_signatures_for<const basic_sender, Env> {
        return {};
    }
#else
    template <::beman::execution26::detail::decays_to<basic_sender> Self, typename Env>
    auto get_completion_signatures(this Self&&, Env&&) noexcept
        -> ::beman::execution26::detail::completion_signatures_for<Self, Env> {
        return {};
    }
#endif
};
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#include <beman/execution26/detail/suppress_pop.hpp>

#endif
