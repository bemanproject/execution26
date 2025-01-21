// include/beman/execution26/detail/continues_on.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONTINUES_ON
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONTINUES_ON

#include <beman/execution26/detail/sender.hpp>
#include <beman/execution26/detail/scheduler.hpp>
#include <beman/execution26/detail/sender_adaptor.hpp>
#include <beman/execution26/detail/get_domain_early.hpp>
#include <beman/execution26/detail/transform_sender.hpp>
#include <beman/execution26/detail/make_sender.hpp>
#include <beman/execution26/detail/sender_for.hpp>
#include <beman/execution26/detail/schedule_from.hpp>
#include <beman/execution26/detail/join_env.hpp>
#include <beman/execution26/detail/fwd_env.hpp>
#include <beman/execution26/detail/get_env.hpp>
#include <beman/execution26/detail/sched_attrs.hpp>
#include <beman/execution26/detail/impls_for.hpp>
#include <beman/execution26/detail/default_impls.hpp>
#include <beman/execution26/detail/get_domain_late.hpp>
#include <beman/execution26/detail/default_domain.hpp>
#include <beman/execution26/detail/query_with_default.hpp>

#include <utility>

// ----------------------------------------------------------------------------

#include <beman/execution26/detail/suppress_push.hpp>

namespace beman::execution26::detail {
// specialize default_domain appropriately
/*!
 * \brief The actual implementation of the continues_on customization point object
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
struct continues_on_t {
    template <::beman::execution26::detail::sender_for<continues_on_t> Sender, typename... Env>
    static auto transform_sender(Sender&& sender, Env&&...) {
        auto&& data{sender.template get<1>()};
        auto&& child{sender.template get<2>()};
        return ::beman::execution26::schedule_from(std::move(data), std::move(child));
    }
    template <::beman::execution26::scheduler Scheduler>
    auto operator()(Scheduler&& scheduler) const {
        return ::beman::execution26::detail::sender_adaptor{*this, ::std::forward<Scheduler>(scheduler)};
    }
    template <::beman::execution26::sender Sender, ::beman::execution26::scheduler Scheduler>
    auto operator()(Sender&& sender, Scheduler&& scheduler) const {
        auto domain(::beman::execution26::detail::get_domain_early(sender));
        return ::beman::execution26::transform_sender(
            domain,
            ::beman::execution26::detail::make_sender(
                *this, std::forward<Scheduler>(scheduler), ::std::forward<Sender>(sender)));
    }
};

/*!
 * \brief Specialization of impls_for to implement the continues_on functionality
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <>
struct impls_for<::beman::execution26::detail::continues_on_t> : ::beman::execution26::detail::default_impls {
    static constexpr auto get_attrs{[](const auto& data, const auto& child) noexcept -> decltype(auto) {
        return ::beman::execution26::detail::join_env(
            ::beman::execution26::detail::sched_attrs(data),
            ::beman::execution26::detail::fwd_env(::beman::execution26::get_env(child)));
    }};
};

/*!
 * \brief Get the sender's domain when the sender is continue_on
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <::beman::execution26::detail::sender_for<::beman::execution26::detail::continues_on_t> Sender, typename Env>
auto get_domain_late(Sender&& sender, Env&&) {
    auto scheduler{sender.template get<1>()};
    return ::beman::execution26::detail::query_with_default(
        ::beman::execution26::get_domain, scheduler, ::beman::execution26::default_domain{});
}
} // namespace beman::execution26::detail

#include <beman/execution26/detail/suppress_pop.hpp>

namespace beman::execution26 {
using ::beman::execution26::detail::continues_on_t;
/*!
 * \brief Customization point object to create a `continues_on` sender.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 */
inline constexpr continues_on_t continues_on{};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
