// include/beman/execution26/detail/meta_contains.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_CONTAINS
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_CONTAINS

#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta {
template <typename T, typename... S>
inline constexpr bool contains{(::std::same_as<T, S> || ...)};
}

// ----------------------------------------------------------------------------

#endif
