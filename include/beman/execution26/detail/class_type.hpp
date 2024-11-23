// include/beman/execution26/detail/class_type.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CLASS_TYPE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CLASS_TYPE

#include <beman/execution26/detail/decays_to.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
/*!
 * \brief Auxiliary concept used to detect class types. [execution.syn#concept:class-type]
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 * \internal
 */
template <typename Tp>
concept class_type = ::beman::execution26::detail::decays_to<Tp, Tp> && ::std::is_class_v<Tp>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
