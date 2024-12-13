// include/beman/execution26/detail/env_of_t.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_ENV_OF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_ENV_OF

#include <beman/execution26/detail/get_env.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
/*!
 * \brief Determine the type of the environment associated with a type
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 */
template <typename T>
using env_of_t = decltype(::beman::execution26::get_env(::std::declval<T>()));
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
