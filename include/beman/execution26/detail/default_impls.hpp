// include/beman/execution26/detail/default_impls.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DEFAULT_IMPLS
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DEFAULT_IMPLS

#include <beman/execution26/detail/allocator_aware_move.hpp>
#include <beman/execution26/detail/callable.hpp>
#include <beman/execution26/detail/empty_env.hpp>
#include <beman/execution26/detail/forward_like.hpp>
#include <beman/execution26/detail/fwd_env.hpp>
#include <beman/execution26/detail/get_allocator.hpp>
#include <beman/execution26/detail/get_env.hpp>
#include <beman/execution26/detail/product_type.hpp>
#include <beman/execution26/detail/sender_decompose.hpp>
#include <beman/execution26/detail/start.hpp>

#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Helper type providing default implementations for basic_sender
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
struct default_impls {
    static constexpr auto get_attrs = [](const auto&, const auto&... child) noexcept -> decltype(auto) {
        if constexpr (1 == sizeof...(child))
            return (::beman::execution26::detail::fwd_env(::beman::execution26::get_env(child)), ...);
        else
            return ::beman::execution26::empty_env{};
    };
    static constexpr auto get_env = [](auto, auto&, const auto& receiver) noexcept -> decltype(auto) {
        return ::beman::execution26::detail::fwd_env(::beman::execution26::get_env(receiver));
    };
    static constexpr auto get_state =
        []<typename Sender, typename Receiver>(Sender&& sender, Receiver& receiver) noexcept -> decltype(auto) {
        auto&& data{[&sender]() -> decltype(auto) {
            if constexpr (requires {
                              sender.size();
                              sender.template get<1>();
                          })
                return sender.template get<1>();
            else
                return ::beman::execution26::detail::get_sender_data(::std::forward<Sender>(sender)).data;
        }()};

        return ::beman::execution26::detail::allocator_aware_move(
            ::beman::execution26::detail::forward_like<Sender>(data), receiver);
    };
    static constexpr auto start = [](auto&, auto&, auto&... ops) noexcept -> void {
        (::beman::execution26::start(ops), ...);
    };
    static constexpr auto complete = []<typename Index, typename Receiver, typename Tag, typename... Args>(
                                         Index, auto&, Receiver& receiver, Tag, Args&&... args) noexcept -> void
        requires ::beman::execution26::detail::callable<Tag, Receiver, Args...>
    {
        static_assert(Index::value == 0);
        Tag()(::std::move(receiver), ::std::forward<Args>(args)...);
    };
};
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
