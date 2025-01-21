// include/beman/execution/detail/sync_wait.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SYNC_WAIT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SYNC_WAIT

#include <beman/execution/detail/as_except_ptr.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/get_delegation_scheduler.hpp>
#include <beman/execution/detail/apply_sender.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/run_loop.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/value_types_of_t.hpp>
#include <beman/execution/detail/decayed_tuple.hpp>
#include <exception>
#include <optional>
#include <utility>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct sync_wait_env {
    ::beman::execution::run_loop* loop{};

    auto query(::beman::execution::get_scheduler_t) const noexcept { return this->loop->get_scheduler(); }
    auto query(::beman::execution::get_delegation_scheduler_t) const noexcept { return this->loop->get_scheduler(); }
};

template <::beman::execution::sender_in<::beman::execution::detail::sync_wait_env> Sender>
using sync_wait_result_type =
    ::std::optional<::beman::execution::value_types_of_t<Sender,
                                                         ::beman::execution::detail::sync_wait_env,
                                                         ::beman::execution::detail::decayed_tuple,
                                                         ::std::type_identity_t>>;

template <typename Sender>
struct sync_wait_state {
    ::beman::execution::run_loop loop{};
    ::std::exception_ptr         error{};

    ::beman::execution::detail::sync_wait_result_type<Sender> result{};
};

template <typename Sender>
struct sync_wait_receiver {
    using receiver_concept = ::beman::execution::receiver_t;

    ::beman::execution::detail::sync_wait_state<Sender>* state{};

    template <typename Error>
    auto set_error(Error&& error) && noexcept -> void {
        this->state->error = ::beman::execution::detail::as_except_ptr(::std::forward<Error>(error));
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

    auto get_env() const noexcept -> ::beman::execution::detail::sync_wait_env {
        return ::beman::execution::detail::sync_wait_env{&this->state->loop};
    }
};

struct sync_wait_t {
    template <typename Sender>
    auto apply_sender(Sender&& sender) const {
        ::beman::execution::detail::sync_wait_state<Sender> state;
        auto op{::beman::execution::connect(::std::forward<Sender>(sender),
                                            ::beman::execution::detail::sync_wait_receiver<Sender>{&state})};
        ::beman::execution::start(op);

        state.loop.run();
        if (state.error) {
            ::std::rethrow_exception(state.error);
        }
        return ::std::move(state.result);
    }

    template <::beman::execution::sender_in<::beman::execution::detail::sync_wait_env> Sender>
        requires requires(Sender&& sender, const sync_wait_t& self) {
            typename ::beman::execution::detail::sync_wait_result_type<Sender>;
            {
                ::beman::execution::apply_sender(
                    ::beman::execution::detail::get_domain_early(std::forward<Sender>(sender)),
                    self,
                    ::std::forward<Sender>(sender))
            } -> ::std::same_as<::beman::execution::detail::sync_wait_result_type<Sender>>;
        }
    auto operator()(Sender&& sender) const {
        auto domain{::beman::execution::detail::get_domain_early(sender)};
        return ::beman::execution::apply_sender(domain, *this, ::std::forward<Sender>(sender));
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
using sync_wait_t = ::beman::execution::detail::sync_wait_t;
inline constexpr ::beman::execution::sync_wait_t sync_wait{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
