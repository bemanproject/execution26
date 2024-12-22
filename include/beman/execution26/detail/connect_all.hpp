// include/beman/execution26/detail/connect_all.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT_ALL
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT_ALL

#include <beman/execution26/detail/connect_result_t.hpp>
#include <beman/execution26/detail/connect.hpp>
#include <beman/execution26/detail/basic_receiver.hpp>
#include <beman/execution26/detail/basic_state.hpp>
#include <beman/execution26/detail/product_type.hpp>
#include <beman/execution26/detail/sender_decompose.hpp>
#include <beman/execution26/detail/forward_like.hpp>
#include <cstddef>
#include <tuple>
#include <utility>

// ----------------------------------------------------------------------------

#include <beman/execution26/detail/suppress_push.hpp>

namespace beman::execution26::detail {
/*!
 * \brief A helper types whose call operator connects all children of a basic_sender
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
struct connect_all_t {
  private:
    template <typename Fun, typename Tuple, ::std::size_t... I>
    static auto apply_with_index_helper(::std::index_sequence<I...> seq, Fun&& fun, Tuple&& tuple) noexcept(noexcept(
        ::std::forward<Fun>(fun)(seq, ::beman::execution26::detail::forward_like<Tuple>(::std::get<I>(tuple))...)))
        -> decltype(auto) {
        return ::std::forward<Fun>(fun)(seq,
                                        ::beman::execution26::detail::forward_like<Tuple>(::std::get<I>(tuple))...);
    }
    template <typename Fun, typename Tuple>
    static auto apply_with_index(Fun&& fun, Tuple&& tuple) noexcept(
        noexcept(apply_with_index_helper(::std::make_index_sequence<::std::tuple_size_v<::std::decay_t<Tuple>>>{},
                                         ::std::forward<Fun>(fun),
                                         ::std::forward<Tuple>(tuple)))) -> decltype(auto) {
        return apply_with_index_helper(::std::make_index_sequence<::std::tuple_size_v<::std::decay_t<Tuple>>>{},
                                       ::std::forward<Fun>(fun),
                                       ::std::forward<Tuple>(tuple));
    }

    template <typename Sender, typename Receiver>
    struct connect_helper {
        ::beman::execution26::detail::basic_state<Sender, Receiver>* op;

        template <::std::size_t... J, typename... C>
        auto operator()(::std::index_sequence<J...>, C&&... c) noexcept(
            (noexcept(::beman::execution26::connect(
                 ::beman::execution26::detail::forward_like<Sender>(c),
                 ::beman::execution26::detail::basic_receiver<Sender, Receiver, ::std::integral_constant<::size_t, J>>{
                     this->op})) &&
             ... && true)) -> decltype(auto) {
            return ::beman::execution26::detail::product_type{::beman::execution26::connect(
                ::beman::execution26::detail::forward_like<Sender>(c),
                ::beman::execution26::detail::basic_receiver<Sender, Receiver, ::std::integral_constant<::size_t, J>>{
                    this->op})...};
        }
    };

    static auto use(auto&&...) {}

  public:
    //-dk:TODO is the S parameter deviating from the spec?
    template <typename Sender, typename S, typename Receiver, ::std::size_t... I>
    auto operator()(::beman::execution26::detail::basic_state<Sender, Receiver>* op,
                    S&&                                                          sender,
                    ::std::index_sequence<I...>) const
        noexcept(noexcept(apply_with_index(
            connect_helper<Sender, Receiver>{op},
            ::beman::execution26::detail::get_sender_data(::std::forward<S>(sender)).children))) -> decltype(auto) {
        return apply_with_index(connect_helper<Sender, Receiver>{op},
                                ::beman::execution26::detail::get_sender_data(::std::forward<S>(sender)).children);
    }
};

/*!
 * \brief A helper object of type connect_all_t
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
inline constexpr connect_all_t connect_all{};
} // namespace beman::execution26::detail

#include <beman/execution26/detail/suppress_pop.hpp>

// ----------------------------------------------------------------------------

#endif
