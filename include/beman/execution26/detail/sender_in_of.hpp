// include/beman/execution26/detail/sender_in_of.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_IN_OF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_IN_OF

#include <beman/execution26/detail/sender_in.hpp>
#include <beman/execution26/detail/matching_sig.hpp>
#include <beman/execution26/detail/set_value.hpp>
#include <beman/execution26/detail/value_types_of_t.hpp>
#include <beman/execution26/detail/value_signature.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <typename Sender, typename Env, typename... A>
concept sender_in_of =
    ::beman::execution26::sender_in<Sender, Env> &&
    ::beman::execution26::detail::matching_sig<
        ::beman::execution26::set_value_t(A...),
        ::beman::execution26::
            value_types_of_t<Sender, Env, ::beman::execution26::detail::value_signature, ::std::type_identity_t> >;
}

// ----------------------------------------------------------------------------

#endif
