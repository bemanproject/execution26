// include/beman/execution26/detail/sender_of.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_OF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_OF

#include <beman/execution26/detail/empty_env.hpp>
#include <beman/execution26/detail/sender_in_of.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <typename Sender, typename... A>
concept sender_of = ::beman::execution26::detail::sender_in_of<Sender, ::beman::execution26::empty_env, A...>;
}

// ----------------------------------------------------------------------------

#endif
