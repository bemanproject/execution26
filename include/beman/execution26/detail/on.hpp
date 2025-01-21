// include/beman/execution26/detail/on.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION26_DETAIL_ON
#define INCLUDED_INCLUDE_BEMAN_EXECUTION26_DETAIL_ON

#include <beman/execution26/detail/scheduler.hpp>
#include <beman/execution26/detail/sender.hpp>
#include <beman/execution26/detail/sender_adaptor_closure.hpp>
#include <beman/execution26/detail/transform_sender.hpp>
#include <beman/execution26/detail/query_with_default.hpp>
#include <beman/execution26/detail/get_domain.hpp>
#include <beman/execution26/detail/get_domain_early.hpp>
#include <beman/execution26/detail/default_domain.hpp>
#include <beman/execution26/detail/make_sender.hpp>
#include <beman/execution26/detail/product_type.hpp>
#include <beman/execution26/detail/sender_for.hpp>
#include <beman/execution26/detail/join_env.hpp>
#include <beman/execution26/detail/forward_like.hpp>
#include <beman/execution26/detail/fwd_env.hpp>
#include <beman/execution26/detail/sched_env.hpp>
#include <beman/execution26/detail/starts_on.hpp>
#include <beman/execution26/detail/continues_on.hpp>
#include <beman/execution26/detail/write_env.hpp>
#include <utility>

#include <beman/execution26/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
struct on_t : ::beman::execution26::sender_adaptor_closure<on_t> {
    template <::beman::execution26::detail::sender_for<on_t> OutSndr, typename Env>
    auto transform_env(OutSndr&& out_sndr, Env&& env) const -> decltype(auto) {
        auto&& data{out_sndr.template get<1>()};

        if constexpr (::beman::execution26::scheduler<decltype(data)>)
            return ::beman::execution26::detail::join_env(
                ::beman::execution26::detail::sched_env(::beman::execution26::detail::forward_like<OutSndr>(data)

                                                            ),
                ::beman::execution26::detail::fwd_env(::std::forward<Env>(env)));
        else
            return std::forward<Env>(env);
    }

    template <typename>
    struct env_needs_get_scheduler {
        using sender_concept = ::beman::execution26::sender_t;
        template <typename Env>
        auto get_completion_signatures(Env&&) const {
            return env_needs_get_scheduler<Env>{};
        }
    };

    template <::beman::execution26::detail::sender_for<on_t> OutSndr, typename Env>
    auto transform_sender(OutSndr&& out_sndr, Env&& env) const -> decltype(auto) {
        struct not_a_scheduler {};
        auto&& [_, data, child] = out_sndr;

        if constexpr (::beman::execution26::scheduler<decltype(data)>) {
            auto sch{::beman::execution26::detail::query_with_default(
                ::beman::execution26::get_scheduler, env, not_a_scheduler{})};
            if constexpr (::std::same_as<not_a_scheduler, decltype(sch)>) {
                return env_needs_get_scheduler<Env>{};
            } else {
                return ::beman::execution26::continues_on(
                    ::beman::execution26::starts_on(::beman::execution26::detail::forward_like<OutSndr>(data),
                                                    ::beman::execution26::detail::forward_like<OutSndr>(child)),
                    ::std::move(sch));
            }
        } else {
            auto& [sch, closure] = data;
            auto orig_sch{::beman::execution26::detail::query_with_default(
                ::beman::execution26::get_completion_scheduler<::beman::execution26::set_value_t>,
                ::beman::execution26::get_env(child),
                ::beman::execution26::detail::query_with_default(
                    ::beman::execution26::get_scheduler, env, not_a_scheduler{}))};

            if constexpr (::std::same_as<not_a_scheduler, decltype(orig_sch)>) {
                return env_needs_get_scheduler<Env>{};
            } else {
                return ::beman::execution26::detail::write_env(
                    ::beman::execution26::continues_on(
                        ::beman::execution26::detail::forward_like<OutSndr>(closure)(
                            ::beman::execution26::continues_on(
                                ::beman::execution26::detail::write_env(
                                    ::beman::execution26::detail::forward_like<OutSndr>(child),
                                    ::beman::execution26::detail::sched_env(orig_sch)),
                                sch)),
                        orig_sch),
                    ::beman::execution26::detail::sched_env(env));
            }
        }
    }

    template <::beman::execution26::scheduler Sch, ::beman::execution26::sender Sndr>
        requires ::beman::execution26::detail::is_sender_adaptor_closure<Sndr>
    auto operator()(Sch&&, Sndr&&) const -> void =
        BEMAN_EXECUTION26_DELETE("on(sch, sndr) requires that sndr isn't both a sender and sender adaptor closure");

    template <::beman::execution26::scheduler                         Sch,
              ::beman::execution26::sender                            Sndr,
              ::beman::execution26::detail::is_sender_adaptor_closure Closure>
        requires ::beman::execution26::detail::is_sender_adaptor_closure<Sndr>
    auto operator()(Sndr&&, Sch&&, Closure&&) const -> void =
        BEMAN_EXECUTION26_DELETE("on(sch, sndr) requires that sndr isn't both a sender and sender adaptor closure");

    template <::beman::execution26::scheduler Sch, ::beman::execution26::sender Sndr>
    auto operator()(Sch&& sch, Sndr&& sndr) const {
        auto domain{::beman::execution26::detail::query_with_default(
            ::beman::execution26::get_domain, sch, ::beman::execution26::default_domain{})};
        return ::beman::execution26::transform_sender(
            domain,
            ::beman::execution26::detail::make_sender(*this, ::std::forward<Sch>(sch), ::std::forward<Sndr>(sndr)));
    }
    template <::beman::execution26::scheduler                         Sch,
              ::beman::execution26::sender                            Sndr,
              ::beman::execution26::detail::is_sender_adaptor_closure Closure>
    auto operator()(Sndr&& sndr, Sch&& sch, Closure&& closure) const {
        auto domain{::beman::execution26::detail::get_domain_early(sndr)};
        return ::beman::execution26::transform_sender(
            domain,
            ::beman::execution26::detail::make_sender(
                *this,
                ::beman::execution26::detail::product_type{::std::forward<Sch>(sch), ::std::forward<Closure>(closure)},
                ::std::forward<Sndr>(sndr)));
    }
    template <::beman::execution26::scheduler Sch, ::beman::execution26::detail::is_sender_adaptor_closure Closure>
    auto operator()(Sch&& sch, Closure&& closure) const {
        return ::beman::execution26::detail::sender_adaptor{
            *this, ::std::forward<Sch>(sch), ::std::forward<Closure>(closure)};
    }
};

#if 0
template <typename Data, typename Sender, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution26::detail::basic_sender<::beman::execution26::detail::on_t, Data, Sender>,
    Env> {
        //-dk:TODO pick up scheduler errors and merge them in?
    using type =
#if 0
    ::beman::execution26::detail::meta::combine<
        ::beman::execution26::completion_signatures_of_t<Sender, Env>,
        ::beman::execution26::completion_signatures<::beman::execution26::set_error_t(::std::exception_ptr)>
        >
#else
        ::beman::execution26::completion_signatures<
            ::beman::execution26::set_value_t(),
            ::beman::execution26::set_error_t(::std::exception_ptr)
            >
#endif
        ;
};
#endif

} // namespace beman::execution26::detail

namespace beman::execution26 {
using on_t = ::beman::execution26::detail::on_t;
inline constexpr ::beman::execution26::on_t on{};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#include <beman/execution26/detail/suppress_pop.hpp>

#endif
