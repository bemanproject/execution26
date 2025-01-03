// include/beman/execution26/detail/sender_adaptor_closure.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_ADAPTOR_CLOSURE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_ADAPTOR_CLOSURE

#include <beman/execution26/detail/sender.hpp>
#include <concepts>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail::pipeable {
struct sender_adaptor_closure_base {};
} // namespace beman::execution26::detail::pipeable

namespace beman::execution26 {
// NOLINTBEGIN(bugprone-crtp-constructor-accessibility)
template <typename>
struct sender_adaptor_closure : ::beman::execution26::detail::pipeable::sender_adaptor_closure_base {};
// NOLINTEND(bugprone-crtp-constructor-accessibility)

} // namespace beman::execution26

namespace beman::execution26::detail {
template <typename Closure>
concept is_sender_adaptor_closure =
    ::std::derived_from<::std::decay_t<Closure>,
                        ::beman::execution26::sender_adaptor_closure<::std::decay_t<Closure>>>;
}

namespace beman::execution26::detail::pipeable {
template <::beman::execution26::sender Sender, typename Adaptor>
    requires(not::beman::execution26::sender<Adaptor>) &&
            ::std::derived_from<::std::decay_t<Adaptor>,
                                ::beman::execution26::sender_adaptor_closure<::std::decay_t<Adaptor>>> &&
            requires(Sender&& sender, Adaptor&& adaptor) {
                { adaptor(::std::forward<Sender>(sender)) } -> ::beman::execution26::sender;
            }
auto operator|(Sender&& sender, Adaptor&& adaptor) {
    return adaptor(::std::forward<Sender>(sender));
}
} // namespace beman::execution26::detail::pipeable

// ----------------------------------------------------------------------------

#endif
