// include/beman/execution26/detail/get_completion_signatures.hpp     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_COMPLETION_SIGNATURES
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_COMPLETION_SIGNATURES

#include <beman/execution26/detail/await_result_type.hpp>
#include <beman/execution26/detail/completion_signatures.hpp>
#include <beman/execution26/detail/env_promise.hpp>
#include <beman/execution26/detail/get_domain_late.hpp> //-dk:TODO remove
#include <beman/execution26/detail/is_awaitable.hpp>
#include <beman/execution26/detail/transform_sender.hpp> //-dk:TODO remove

#include <concepts>
#include <exception>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
struct get_completion_signatures_t {
  private:
    template <typename Sender, typename Env>
    static auto get(Sender&& sender, Env&& env) noexcept {
        auto new_sender{[](auto&& sndr, auto&& e) -> decltype(auto) {
            auto domain{::beman::execution26::detail::get_domain_late(sndr, e)};
            return ::beman::execution26::transform_sender(
                domain, ::std::forward<Sender>(sndr), ::std::forward<Env>(e));
        }};

        using sender_type = ::std::remove_cvref_t<decltype(new_sender(sender, env))>;
        using decayed_env = ::std::remove_cvref_t<Env>;
        if constexpr (requires { new_sender(sender, env).get_completion_signatures(env); })
            return decltype(new_sender(sender, env).get_completion_signatures(env)){};
        else if constexpr (requires { typename sender_type::completion_signatures; })
            return typename sender_type::completion_signatures{};
        else if constexpr (::beman::execution26::detail::
                               is_awaitable<sender_type, ::beman::execution26::detail::env_promise<decayed_env>>) {
            using result_type = ::beman::execution26::detail::
                await_result_type<sender_type, ::beman::execution26::detail::env_promise<decayed_env>>;
            if constexpr (::std::same_as<void, result_type>) {
                return ::beman::execution26::completion_signatures<::beman::execution26::set_value_t(),
                                                                   ::beman::execution26::set_error_t(
                                                                       ::std::exception_ptr),
                                                                   ::beman::execution26::set_stopped_t()>{};
            } else {
                return ::beman::execution26::completion_signatures<::beman::execution26::set_value_t(result_type),
                                                                   ::beman::execution26::set_error_t(
                                                                       ::std::exception_ptr),
                                                                   ::beman::execution26::set_stopped_t()>{};
            }
        }
    }

  public:
    template <typename Sender, typename Env>
        requires(not::std::same_as<void,
                                   decltype(get_completion_signatures_t::get(::std::declval<Sender>(),
                                                                             ::std::declval<Env>()))>)
    auto operator()(Sender&& sender, Env&& env) const noexcept {
        return this->get(::std::forward<Sender>(sender), ::std::forward<Env>(env));
    }
};
inline constexpr get_completion_signatures_t get_completion_signatures{};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
