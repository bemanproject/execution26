// include/beman/execution26/detail/get_completion_behaviour.hpp      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_COMPLETION_BEHAVIOUR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_COMPLETION_BEHAVIOUR

#include <beman/execution26/detail/completion_behaviour.hpp>
#include <beman/execution26/detail/forward_like.hpp>
#include <beman/execution26/detail/is_awaitable.hpp>
#include <beman/execution26/detail/get_domain_late.hpp>
#include <beman/execution26/detail/transform_sender.hpp>

#include <type_traits>

namespace beman::execution26 {

struct get_completion_behaviour_t {
    template <class Sender>
    constexpr auto operator()(Sender&& sender) const noexcept {
        if constexpr (requires { typename ::std::remove_cvref_t<Sender>::completion_behaviour; }) {
            return typename ::std::remove_cvref_t<Sender>::completion_behaviour{};
        } else if constexpr (requires { ::std::forward<Sender>(sender).get_completion_behaviour(); }) {
            return ::std::forward<Sender>(sender).get_completion_behaviour();
        } else if constexpr (::beman::execution26::detail::is_awaitable<
                                 ::std::remove_cvref_t<Sender>,
                                 ::beman::execution26::detail::env_promise<::beman::execution26::empty_env>>) {
            return sender.await_ready() ? ::beman::execution26::completion_behaviour::inline_completion
                                        : ::beman::execution26::completion_behaviour::unknown;
        } else {
            return ::beman::execution26::completion_behaviour::unknown;
        }
    }

    template <class Sender, class Env>
    constexpr auto operator()(Sender&& sender, Env&& env) const noexcept {
        auto new_sender{[](auto&& sndr, auto&& e) -> decltype(auto) {
            auto domain{::beman::execution26::detail::get_domain_late(sndr, e)};
            return ::beman::execution26::transform_sender(
                domain, ::std::forward<Sender>(sndr), ::std::forward<Env>(e));
        }};

        using new_sender_type =
            ::std::remove_cvref_t<decltype(new_sender(::std::forward<Sender>(sender), ::std::forward<Env>(env)))>;
        using decayed_env = ::std::remove_cvref_t<Env>;

        if constexpr (requires { typename new_sender_type::completion_behaviour; }) {
            return typename new_sender_type::completion_behaviour{};
        } else if constexpr (requires {
                                 new_sender(::std::forward<Sender>(sender), ::std::forward<Env>(env))
                                     .get_completion_behaviour(std::forward<Env>(env));
                             }) {
            return new_sender(::std::forward<Sender>(sender), ::std::forward<Env>(env))
                .get_completion_behaviour(std::forward<Env>(env));
        } else if constexpr (::beman::execution26::detail::is_awaitable<
                                 new_sender_type,
                                 ::beman::execution26::detail::env_promise<decayed_env>>) {
            if (new_sender(::std::forward<Sender>(sender), ::std::forward<Env>(env)).await_ready()) {
                return completion_behaviour::inline_completion;
            } else {
                return completion_behaviour::unknown;
            }
        } else {
            return completion_behaviour::unknown;
        }
    }
};

inline constexpr get_completion_behaviour_t get_completion_behaviour{};

} // namespace beman::execution26

#endif
