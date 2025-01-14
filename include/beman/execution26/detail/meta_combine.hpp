// include/beman/execution26/detail/meta_combine.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_COMBINE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_COMBINE

#include <beman/execution26/detail/type_list.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta::detail {
template <typename...>
struct combine;

template <template <typename...> class L0, typename... T0>
struct combine<L0<T0...>> {
    using type = L0<T0...>;
};
template <template <typename...> class L0,
          typename... T0,
          template <typename...> class L1,
          typename... T1,
          typename... L>
struct combine<L0<T0...>, L1<T1...>, L...> {
    using type = typename combine<L0<T0..., T1...>, L...>::type;
};
} // namespace beman::execution26::detail::meta::detail

namespace beman::execution26::detail::meta {
template <typename... L>
using combine = typename ::beman::execution26::detail::meta::detail::combine<L...>::type;
}

// ----------------------------------------------------------------------------

#endif
