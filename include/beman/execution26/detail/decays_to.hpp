// include/beman/execution26/detail/decays_to.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYS_TO
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYS_TO

#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Helper concept to determine if the first argument decays to the second argument
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <typename From, typename To>
concept decays_to = ::std::same_as<::std::decay_t<From>, To>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
