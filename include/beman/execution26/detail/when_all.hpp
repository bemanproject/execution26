// include/beman/execution26/detail/when_all.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_WHEN_ALL
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_WHEN_ALL

#include <beman/execution26/detail/completion_signatures_of_t.hpp>
#include <beman/execution26/detail/decayed_tuple.hpp>
#include <beman/execution26/detail/decayed_type_list.hpp>
#include <beman/execution26/detail/default_domain.hpp>
#include <beman/execution26/detail/default_impls.hpp>
#include <beman/execution26/detail/empty_env.hpp>
#include <beman/execution26/detail/env_of_t.hpp>
#include <beman/execution26/detail/error_types_of_t.hpp>
#include <beman/execution26/detail/get_domain.hpp>
#include <beman/execution26/detail/get_domain_early.hpp>
#include <beman/execution26/detail/get_stop_token.hpp>
#include <beman/execution26/detail/impls_for.hpp>
#include <beman/execution26/detail/inplace_stop_source.hpp>
#include <beman/execution26/detail/join_env.hpp>
#include <beman/execution26/detail/make_env.hpp>
#include <beman/execution26/detail/make_sender.hpp>
#include <beman/execution26/detail/meta_combine.hpp>
#include <beman/execution26/detail/meta_size.hpp>
#include <beman/execution26/detail/meta_to.hpp>
#include <beman/execution26/detail/meta_transform.hpp>
#include <beman/execution26/detail/meta_prepend.hpp>
#include <beman/execution26/detail/meta_unique.hpp>
#include <beman/execution26/detail/on_stop_request.hpp>
#include <beman/execution26/detail/sender.hpp>
#include <beman/execution26/detail/sender_in.hpp>
#include <beman/execution26/detail/set_value.hpp>
#include <beman/execution26/detail/stop_callback_for_t.hpp>
#include <beman/execution26/detail/stop_token_of_t.hpp>
#include <beman/execution26/detail/transform_sender.hpp>
#include <beman/execution26/detail/type_list.hpp>
#include <beman/execution26/detail/value_types_of_t.hpp>

#include <concepts>
#include <exception>
#include <optional>
#include <variant>
#include <tuple>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
struct when_all_t {
    template <::beman::execution26::sender... Sender>
        requires(0u != sizeof...(Sender)) &&
                ((::beman::execution26::detail::meta::size_v<
                      ::beman::execution26::value_types_of_t<Sender,
                                                             ::beman::execution26::empty_env,
                                                             ::std::tuple,
                                                             ::beman::execution26::detail::type_list>> == 1u) &&
                 ...) &&
                requires(Sender&&... s) {
                    typename ::std::common_type_t<decltype(::beman::execution26::detail::get_domain_early(s))...>;
                }
    auto operator()(Sender&&... sender) const {
        using common_t =
            typename ::std::common_type_t<decltype(::beman::execution26::detail::get_domain_early(sender))...>;
        return ::beman::execution26::transform_sender(
            common_t(), ::beman::execution26::detail::make_sender(*this, {}, ::std::forward<Sender>(sender)...));
    }
};

template <typename>
struct when_all_value_types;
template <typename... T>
struct when_all_value_types<::beman::execution26::detail::type_list<T...>> {
    using type = ::beman::execution26::completion_signatures<::beman::execution26::set_value_t(T...)>;
};

template <>
struct impls_for<::beman::execution26::detail::when_all_t> : ::beman::execution26::detail::default_impls {
    static constexpr auto get_attrs{[](auto&&, auto&&... sender) {
        using common_t =
            typename ::std::common_type_t<decltype(::beman::execution26::detail::get_domain_early(sender))...>;
        if constexpr (::std::same_as<common_t, ::beman::execution26::default_domain>)
            return ::beman::execution26::empty_env{};
        else
            return ::beman::execution26::detail::make_env(::beman::execution26::get_domain, common_t{});
    }};
    static constexpr auto get_env{[]<typename State, typename Receiver>(
                                      auto&&, State& state, const Receiver& receiver) noexcept {
        return ::beman::execution26::detail::join_env(
            ::beman::execution26::detail::make_env(::beman::execution26::get_stop_token, state.stop_src.get_token()),
            ::beman::execution26::get_env(receiver));
    }};

    enum class disposition : unsigned char { started, error, stopped };

    template <typename Receiver, typename... Sender>
    struct state_type {
        struct nonesuch {};
        using env_t        = ::beman::execution26::env_of_t<Receiver>;
        using values_tuple = ::std::tuple<
            ::beman::execution26::
                value_types_of_t<Sender, env_t, ::beman::execution26::detail::decayed_tuple, ::std::optional>...>;
        using errors_variant = ::beman::execution26::detail::meta::to<
            ::std::variant,
            ::beman::execution26::detail::meta::unique<::beman::execution26::detail::meta::prepend<
                nonesuch,
                ::beman::execution26::detail::meta::prepend<
                    ::std::exception_ptr,
                    ::beman::execution26::detail::meta::combine<::beman::execution26::detail::meta::to<
                        ::beman::execution26::detail::type_list,
                        ::beman::execution26::detail::meta::combine<::beman::execution26::error_types_of_t<
                            Sender,
                            env_t,
                            ::beman::execution26::detail::decayed_type_list>...>>>>>>>;
        using stop_callback = ::beman::execution26::stop_callback_for_t<
            ::beman::execution26::stop_token_of_t<::beman::execution26::env_of_t<Receiver>>,
            ::beman::execution26::detail::on_stop_request<state_type>>;

        void arrive(Receiver& recvr) noexcept {
            if (0u == --count)
                this->complete(recvr);
        }

        void complete(Receiver& recvr) noexcept {
            switch (this->disp) {
            case disposition::started: {
                auto tie = []<typename... T>(::std::tuple<T...>& t) noexcept {
                    return ::std::apply([](auto&... a) { return ::std::tie(a...); }, t);
                };
                auto set = [&](auto&... t) noexcept {
                    ::beman::execution26::set_value(::std::move(recvr), ::std::move(t)...);
                };

                this->on_stop.reset();
                ::std::apply([&](auto&... opts) noexcept { ::std::apply(set, ::std::tuple_cat(tie(*opts)...)); },
                             this->values);
            } break;
            case disposition::error:
                this->on_stop.reset();
                try {
                    ::std::visit(
                        [&]<typename Error>(Error& error) noexcept {
                            if constexpr (!::std::same_as<Error, nonesuch>) {
                                ::beman::execution26::set_error(::std::move(recvr), ::std::move(error));
                            }
                        },
                        this->errors);
                } catch (...) {
                    ::beman::execution26::set_error(::std::move(recvr), ::std::current_exception());
                }
                break;
            case disposition::stopped:
                this->on_stop.reset();
                ::beman::execution26::set_stopped(::std::move(recvr));
                break;
            }
        }

        auto request_stop() -> void {
            if (1u == ++this->count)
                --this->count;
            else {
                this->stop_src.request_stop();
                this->arrive(*this->receiver);
            }
        }

        Receiver*                                 receiver{};
        ::std::atomic<size_t>                     count{sizeof...(Sender)};
        ::beman::execution26::inplace_stop_source stop_src{};
        ::std::atomic<disposition>                disp{disposition::started};
        errors_variant                            errors{};
        values_tuple                              values{};
        ::std::optional<stop_callback>            on_stop{::std::nullopt};
    };

    template <typename Receiver>
    struct make_state {
        template <::beman::execution26::sender_in<::beman::execution26::env_of_t<Receiver>>... Sender>
        auto operator()(auto, auto, Sender&&...) const {
            return state_type<Receiver, Sender...>{};
        }
    };
    static constexpr auto get_state{[]<typename Sender, typename Receiver>(Sender&& sender, Receiver&) noexcept(
                                        noexcept(std::forward<Sender>(sender).apply(make_state<Receiver>{}))) {
        return std::forward<Sender>(sender).apply(make_state<Receiver>{});
    }};
    static constexpr auto start{[]<typename State, typename Receiver, typename... Ops>(
                                    State& state, Receiver& receiver, Ops&... ops) noexcept -> void {
        state.receiver = &receiver;
        state.on_stop.emplace(::beman::execution26::get_stop_token(::beman::execution26::get_env(receiver)),
                              ::beman::execution26::detail::on_stop_request{state});
        if (state.stop_src.stop_requested()) {
            state.on_stop.reset();
            ::beman::execution26::set_stopped(std::move(receiver));
        } else {
            (::beman::execution26::start(ops), ...);
        }
    }};
    static constexpr auto complete{
        []<typename Index, typename State, typename Receiver, typename Set, typename... Args>(
            Index, State& state, Receiver& receiver, Set, Args&&... args) noexcept -> void {
            if constexpr (::std::same_as<Set, ::beman::execution26::set_error_t>) {
                if (disposition::error != state.disp.exchange(disposition::error)) {
                    state.stop_src.request_stop();
                    try {
                        state.errors.template emplace<typename ::std::decay<Args...>::type>(
                            ::std::forward<Args>(args)...);
                    } catch (...) {
                        state.errors.template emplace<::std::exception_ptr>(::std::current_exception());
                    }
                }
            } else if constexpr (::std::same_as<Set, ::beman::execution26::set_stopped_t>) {
                auto expected = disposition::started;
                if (state.disp.compare_exchange_strong(expected, disposition::stopped)) {
                    state.stop_src.request_stop();
                }
            } else if constexpr (!::std::same_as<decltype(State::values), ::std::tuple<>>) {
                if (state.disp == disposition::started) {
                    auto& opt = ::std::get<Index::value>(state.values);
                    try {
                        opt.emplace(::std::forward<Args>(args)...);
                    } catch (...) {
                        if (disposition::error != state.disp.exchange(disposition::error)) {
                            state.stop_src.request_stop();
                            state.errors.template emplace<::std::exception_ptr>(::std::current_exception());
                        }
                    }
                }
            }
            state.arrive(receiver);
        }};
};

template <typename Data, typename Env, typename... Sender>
struct completion_signatures_for_impl<
    ::beman::execution26::detail::basic_sender<::beman::execution26::detail::when_all_t, Data, Sender...>,
    Env> {
    template <typename... E>
    struct error_comps_t {
        using type = ::beman::execution26::completion_signatures<::beman::execution26::set_error_t(E)...>;
    };
    template <typename... E>
    using error_comps = typename error_comps_t<E...>::type;

    using value_types =
        typename ::beman::execution26::detail::when_all_value_types<::beman::execution26::detail::meta::combine<
            ::beman::execution26::
                value_types_of_t<Sender, Env, ::beman::execution26::detail::type_list, ::std::type_identity_t>...>>::
            type;
    using error_types = ::beman::execution26::detail::meta::unique<::beman::execution26::detail::meta::combine<
        ::beman::execution26::error_types_of_t<Sender, Env, error_comps>...>>;
    using type        = ::beman::execution26::detail::meta::combine<value_types, error_types>;
};
} // namespace beman::execution26::detail

namespace beman::execution26 {
using ::beman::execution26::detail::when_all_t;
inline constexpr ::beman::execution26::when_all_t when_all{};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
