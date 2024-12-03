// include/beman/execution26/detail/value_signature.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_VALUE_SIGNATURE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_VALUE_SIGNATURE

#include <beman/execution26/detail/set_value.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <typename... A>
using value_signature = ::beman::execution26::set_value_t(A...);
}

// ----------------------------------------------------------------------------

#endif
