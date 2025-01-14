// include/beman/execution26/detail/meta_prepend.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_PREPEND
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_PREPEND

// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta::detail {
template <typename, typename>
struct prepend;

template <template <typename...> class List, typename H, typename... T>
struct prepend<H, List<T...>> {
    using type = List<H, T...>;
};
} // namespace beman::execution26::detail::meta::detail

namespace beman::execution26::detail::meta {
template <typename H, typename Tail>
using prepend = typename ::beman::execution26::detail::meta::detail::prepend<H, Tail>::type;
}

// ----------------------------------------------------------------------------

#endif
