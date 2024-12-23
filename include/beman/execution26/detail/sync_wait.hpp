// include/beman/execution26/detail/sync_wait.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SYNC_WAIT
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SYNC_WAIT

#include <beman/execution26/detail/as_except_ptr.hpp>
#include <beman/execution26/detail/sender_in.hpp>
#include <beman/execution26/detail/get_domain_early.hpp>
#include <beman/execution26/detail/get_scheduler.hpp>
#include <beman/execution26/detail/get_delegation_scheduler.hpp>
#include <beman/execution26/detail/apply_sender.hpp>
#include <beman/execution26/detail/connect.hpp>
#include <beman/execution26/detail/start.hpp>
#include <beman/execution26/detail/run_loop.hpp>
#include <beman/execution26/detail/receiver.hpp>
#include <beman/execution26/detail/sender_in.hpp>
#include <beman/execution26/detail/value_types_of_t.hpp>
#include <beman/execution26/detail/decayed_tuple.hpp>
#include <exception>
#include <optional>
#include <utility>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
struct sync_wait_env {
    ::beman::execution26::run_loop* loop{};

    auto query(::beman::execution26::get_scheduler_t) const noexcept { return this->loop->get_scheduler(); }
    auto query(::beman::execution26::get_delegation_scheduler_t) const noexcept { return this->loop->get_scheduler(); }
};

template <::beman::execution26::sender_in<::beman::execution26::detail::sync_wait_env> Sender>
using sync_wait_result_type =
    ::std::optional<::beman::execution26::value_types_of_t<Sender,
                                                           ::beman::execution26::detail::sync_wait_env,
                                                           ::beman::execution26::detail::decayed_tuple,
                                                           ::std::type_identity_t>>;

template <typename Sender>
struct sync_wait_state {
    ::beman::execution26::run_loop loop{};
    ::std::exception_ptr           error{};

    ::beman::execution26::detail::sync_wait_result_type<Sender> result{};
};

template <typename Sender>
struct sync_wait_receiver {
    using receiver_concept = ::beman::execution26::receiver_t;

    ::beman::execution26::detail::sync_wait_state<Sender>* state{};

    template <typename Error>
    auto set_error(Error&& error) && noexcept -> void {
        this->state->error = ::beman::execution26::detail::as_except_ptr(::std::forward<Error>(error));
        this->state->loop.finish();
    }
    auto set_stopped() && noexcept -> void { this->state->loop.finish(); }
    template <typename... Args>
    auto set_value(Args&&... args) && noexcept -> void {
        try {
            this->state->result.emplace(::std::forward<Args>(args)...);
        } catch (...) {
            this->state->error = ::std::current_exception();
        }
        this->state->loop.finish();
    }
    auto get_env() const noexcept -> ::beman::execution26::detail::sync_wait_env {
        return ::beman::execution26::detail::sync_wait_env{&this->state->loop};
    }
};

struct sync_wait_t {
    template <typename Sender>
    auto apply_sender(Sender&& sender) {
        ::beman::execution26::detail::sync_wait_state<Sender> state;
        auto op{::beman::execution26::connect(::std::forward<Sender>(sender),
                                              ::beman::execution26::detail::sync_wait_receiver<Sender>{&state})};
        ::beman::execution26::start(op);

        state.loop.run();
        if (state.error) {
            ::std::rethrow_exception(state.error);
        }
        return ::std::move(state.result);
    }

    template <::beman::execution26::sender_in<::beman::execution26::detail::sync_wait_env> Sender>
        requires requires(Sender&& sender, const sync_wait_t& self) {
            typename ::beman::execution26::detail::sync_wait_result_type<Sender>;
            {
                ::beman::execution26::apply_sender(
                    ::beman::execution26::detail::get_domain_early(sender), self, ::std::forward<Sender>(sender))
            } -> ::std::same_as<::beman::execution26::detail::sync_wait_result_type<Sender>>;
        }
    auto operator()(Sender&& sender) const {
        auto domain{::beman::execution26::detail::get_domain_early(sender)};
        return ::beman::execution26::apply_sender(domain, *this, ::std::forward<Sender>(sender));
    }
};
} // namespace beman::execution26::detail

namespace beman::execution26 {
using sync_wait_t = ::beman::execution26::detail::sync_wait_t;
inline constexpr ::beman::execution26::sync_wait_t sync_wait{};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
