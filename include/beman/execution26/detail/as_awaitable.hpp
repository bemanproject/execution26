// include/beman/execution26/detail/as_awaitable.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_AS_AWAITABLE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_AS_AWAITABLE

#include <beman/execution26/detail/env_promise.hpp>
#include <beman/execution26/detail/single_sender.hpp>
#include <beman/execution26/detail/single_sender_value_type.hpp>
#include <beman/execution26/detail/connect_result_t.hpp>

#include <concepts>
#include <coroutine>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
namespace detail {
template <class Sndr, class Promise>
concept awaitable_sender = single_sender<Sndr, env_of_t<Promise>> && requires(Promise& prom) {
    { p.unhandled_stop() } -> ::std::convertible_to<::std::coroutine_handle<>>
};

template <class Sndr, class Promise>
class sender_awaitable {
    struct unit {};
    using value_type   = single_sender_value_type<Sndr, env_of_t<Promise>>;
    using result_type  = conditional_t<::std::is_void_v<value_type>, unit, value_type>;
    using variant_type = ::std::variant<::std::monostate, result_type, ::std::exception_ptr>;
    struct awaitable_receiver {
        using receiver_concept = beman::execution26::receiver_t;

        ::std::variant<::std::monostate, result_type, ::std::exception_ptr>* result_ptr_;
        ::std::coroutine_handle<>                                            continuation_;
    };
    using op_state_type = ::beman::execution26::connect_result_t<Sndr, awaitable_receiver>;

    variant_type  result{};
    op_state_type state;

  public:
    sender_awaitable(Sndr&& sndr, Promise& p)
        : state{::beman::execution26::connect(
              ::std::forward<Sndr>(sndr),
              awaitable_receiver{::std::addressof(result), ::std::coroutine_handle<Promise>::from_promise(p)})} {}

    static constexpr bool await_ready() noexcept { return false; }
    void       await_suspend(::std::coroutine_handle<Promise>) noexcept { ::beman::execution26::start(state); }
    value_type await_resume() {
        if (::std::holds_alternative<::std::exception_ptr>(result)) {
            ::std::rethrow_exception(::std::get<::std::exception_ptr>(result));
        }
        if constexpr (::std::is_void_v<value_type>) {
            return;
        } else {
            return ::std::get<value_type>(result);
        }
    }
};
} // namespace detail

/*!
 * \brief Turn an entity, e.g., a sender, into an awaitable.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 */
struct as_awaitable_t {
    template <typename E, typename P>
    auto operator()(E&& e, P& p) const {
        if constexpr (requires { ::std::forward<E>(e).as_awaitable(p); }) {
            //-dk:TODO mandates is-awaitable<E, Promise>
            return ::std::forward<E>(e).as_awaitable(p);
        }
        //-dk:TODO deal with other cases
    }
};
inline constexpr ::beman::execution26::as_awaitable_t as_awaitable{};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
