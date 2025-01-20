// include/beman/execution26/detail/decayed_typeof.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_TYPEOF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_TYPEOF

#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Helper type alias to get the type after decaying the argument
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <auto&& Tag>
using decayed_typeof = ::std::decay_t<decltype(Tag)>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
