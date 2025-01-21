// include/beman/execution/detail/empty_env.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_EMPTY_ENV
#define INCLUDED_BEMAN_EXECUTION_DETAIL_EMPTY_ENV

#include <beman/execution/detail/common.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution {
/*!
 * \brief An empty type used as an environment when there is no environment available
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 */
struct empty_env {};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
