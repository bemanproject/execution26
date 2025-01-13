// include/beman/execution26/detail/schedule_from.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SCHEDULE_FROM
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SCHEDULE_FROM

#include <beman/execution26/detail/child_type.hpp>
#include <beman/execution26/detail/meta_combine.hpp>
#include <beman/execution26/detail/completion_signatures_of_t.hpp>
#include <beman/execution26/detail/connect.hpp>
#include <beman/execution26/detail/decayed_tuple.hpp>
#include <beman/execution26/detail/default_domain.hpp>
#include <beman/execution26/detail/default_impls.hpp>
#include <beman/execution26/detail/error_types_of_t.hpp>
#include <beman/execution26/detail/env_of_t.hpp>
#include <beman/execution26/detail/fwd_env.hpp>
#include <beman/execution26/detail/get_domain.hpp>
#include <beman/execution26/detail/get_env.hpp>
#include <beman/execution26/detail/impls_for.hpp>
#include <beman/execution26/detail/join_env.hpp>
#include <beman/execution26/detail/make_sender.hpp>
#include <beman/execution26/detail/meta_prepend.hpp>
#include <beman/execution26/detail/meta_to.hpp>
#include <beman/execution26/detail/meta_transform.hpp>
#include <beman/execution26/detail/query_with_default.hpp>
#include <beman/execution26/detail/sched_attrs.hpp>
#include <beman/execution26/detail/schedule.hpp>
#include <beman/execution26/detail/schedule_result_t.hpp>
#include <beman/execution26/detail/scheduler.hpp>
#include <beman/execution26/detail/sender.hpp>
#include <beman/execution26/detail/sender_in.hpp>
#include <beman/execution26/detail/set_error.hpp>
#include <beman/execution26/detail/set_stopped.hpp>
#include <beman/execution26/detail/start.hpp>
#include <beman/execution26/detail/transform_sender.hpp>
#include <beman/execution26/detail/meta_unique.hpp>

#include <exception>
#include <type_traits>
#include <utility>
#include <variant>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Turn a completion signatures into a std::tuple type.
 * \internal
 */
template <typename>
struct as_tuple;
/*!
 * \brief The actual operational partial specialization of as_tuple.
 * \internal
 */
template <typename Tag, typename... T>
struct as_tuple<Tag(T...)> {
    using type = ::beman::execution26::detail::decayed_tuple<Tag, T...>;
};
template <typename T>
using as_tuple_t = typename as_tuple<T>::type;

struct schedule_from_t {
    template <::beman::execution26::scheduler Scheduler, ::beman::execution26::sender Sender>
    auto operator()(Scheduler&& scheduler, Sender&& sender) const {
        auto domain{::beman::execution26::detail::query_with_default(
            ::beman::execution26::get_domain, scheduler, ::beman::execution26::default_domain{})};
        return ::beman::execution26::transform_sender(
            domain,
            ::beman::execution26::detail::make_sender(
                *this, ::std::forward<Scheduler>(scheduler), ::std::forward<Sender>(sender)));
    }
};

template <>
struct impls_for<::beman::execution26::detail::schedule_from_t> : ::beman::execution26::detail::default_impls {
    template <typename State>
    struct upstream_receiver {
        using receiver_concept = ::beman::execution26::receiver_t;
        State* state;

        auto set_value() && noexcept -> void {
            try {
                ::std::visit(
                    [this]<typename Tuple>(Tuple& result) noexcept -> void {
                        if constexpr (not::std::same_as<::std::monostate, Tuple>) {
                            ::std::apply(
                                [this](auto&& tag, auto&&... args) {
                                    tag(::std::move(this->state->receiver), ::std::move(args)...);
                                },
                                result);
                        }
                    },
                    state->async_result);
            } catch (...) {
                ::beman::execution26::set_error(::std::move(state->receiver), ::std::current_exception());
            }
        }

        template <typename Error>
        auto set_error(Error&& err) && noexcept -> void {
            ::beman::execution26::set_error(std::move(state->receiver), std::forward<Error>(err));
        }

        auto set_stopped() && noexcept -> void { ::beman::execution26::set_stopped(std::move(state->receiver)); }

        auto get_env() const noexcept -> decltype(auto) {
            return ::beman::execution26::detail::fwd_env(::beman::execution26::get_env(state->receiver));
        }
    };
    template <typename Receiver, typename Variant>
    struct state_base {
        Receiver receiver;
        Variant  async_result{};
    };
    template <typename Receiver, typename Scheduler, typename Variant>
    struct state_type : state_base<Receiver, Variant> {
        using receiver_t = upstream_receiver<state_base<Receiver, Variant>>;
        using operation_t =
            ::beman::execution26::connect_result_t<::beman::execution26::schedule_result_t<Scheduler>, receiver_t>;
        operation_t op_state;

        static constexpr bool nothrow() {
            return noexcept(::beman::execution26::connect(::beman::execution26::schedule(::std::declval<Scheduler>()),
                                                          receiver_t{nullptr}));
        }
        explicit state_type(Scheduler& sch, Receiver& rcvr) noexcept(nothrow())
            : state_base<Receiver, Variant>{rcvr},
              op_state(::beman::execution26::connect(::beman::execution26::schedule(sch), receiver_t{this})) {}
    };

    static constexpr auto get_attrs{[](const auto& data, const auto& child) noexcept -> decltype(auto) {
        return ::beman::execution26::detail::join_env(
            ::beman::execution26::detail::sched_attrs(data),
            ::beman::execution26::detail::fwd_env(::beman::execution26::get_env(child)));
    }};
    static constexpr auto get_state{
        []<typename Sender, typename Receiver>(Sender&& sender, Receiver& receiver) //-dk:TODO noexcept(see below)
            requires ::beman::execution26::sender_in<::beman::execution26::detail::child_type<Sender>,
                                                     ::beman::execution26::env_of_t<Receiver>>
        {
            auto sch{sender.template get<1>()};

            using sched_t   = ::std::remove_cvref_t<decltype(sch)>;
            using variant_t = ::beman::execution26::detail::meta::unique<::beman::execution26::detail::meta::prepend<
                ::std::monostate,
                ::beman::execution26::detail::meta::transform<
                    ::beman::execution26::detail::as_tuple_t,
                    ::beman::execution26::detail::meta::to<
                        ::std::variant,
                        ::beman::execution26::detail::meta::combine<
                            ::beman::execution26::completion_signatures_of_t<
                                ::beman::execution26::detail::child_type<Sender>,
                                ::beman::execution26::env_of_t<Receiver>>,
                            //-dk:TODO get proper error completion signatures
                            ::beman::execution26::completion_signatures<::beman::execution26::set_error_t(
                                ::std::exception_ptr)>>>>>>;

            return state_type<Receiver, sched_t, variant_t>(sch, receiver);
        }};
    static constexpr auto complete{
        []<typename Tag, typename... Args>(auto, auto& state, auto& receiver, Tag, Args&&... args) noexcept -> void {
            using result_t         = ::beman::execution26::detail::decayed_tuple<Tag, Args...>;
            constexpr bool nothrow = ::std::is_nothrow_constructible_v<result_t, Tag, Args...>;

            try {
                [&]() noexcept(nothrow) {
                    state.async_result.template emplace<result_t>(Tag(), std::forward<Args>(args)...);
                }();
            } catch (...) {
                if constexpr (not nothrow)
                    ::beman::execution26::set_error(::std::move(receiver), ::std::current_exception());
            }

            if (state.async_result.index() == 0)
                return;

            ::beman::execution26::start(state.op_state);
        }};
};

template <typename Scheduler, typename Sender, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution26::detail::basic_sender<::beman::execution26::detail::schedule_from_t, Scheduler, Sender>,
    Env> {
    using scheduler_sender = decltype(::beman::execution26::schedule(::std::declval<Scheduler>()));
    template <typename... E>
    using as_set_error = ::beman::execution26::completion_signatures<::beman::execution26::set_error_t(E)...>;
    using type         = ::beman::execution26::detail::meta::combine<
                decltype(::beman::execution26::get_completion_signatures(::std::declval<Sender>(), ::std::declval<Env>())),
                ::beman::execution26::error_types_of_t<scheduler_sender, Env, as_set_error>,
                ::beman::execution26::completion_signatures<::beman::execution26::set_error_t(
            ::std::exception_ptr)> //-dk:TODO this one should be deduced
                >;
};
} // namespace beman::execution26::detail

namespace beman::execution26 {
using schedule_from_t = beman::execution26::detail::schedule_from_t;
inline constexpr ::beman::execution26::schedule_from_t schedule_from{};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
