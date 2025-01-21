// include/beman/execution26/detail/completion_tag.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_TAG
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_TAG

#include <beman/execution26/detail/set_error.hpp>
#include <beman/execution26/detail/set_stopped.hpp>
#include <beman/execution26/detail/set_value.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief A helper concept used to determine if a type is one of the completion tags.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <typename Tag>
concept completion_tag =
    ::std::same_as<Tag, ::beman::execution26::set_error_t> ||
    ::std::same_as<Tag, ::beman::execution26::set_stopped_t> || ::std::same_as<Tag, ::beman::execution26::set_value_t>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
