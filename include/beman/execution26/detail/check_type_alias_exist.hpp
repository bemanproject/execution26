// include/beman/execution26/detail/check_type_alias_exist.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CHECK_TYPE_ALIAS_EXIST
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CHECK_TYPE_ALIAS_EXIST

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Auxiliary class template used to detect whether a type alias exist within a class.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <template <typename> class>
struct check_type_alias_exist;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
