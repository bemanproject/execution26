// include/beman/execution26/detail/decayed_type_list.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_TYPE_LIST
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_TYPE_LIST

#include "beman/execution26/detail/type_list.hpp"

#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <class... Args>
using decayed_type_list = ::beman::execution26::detail::type_list<::std::decay_t<Args>...>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
