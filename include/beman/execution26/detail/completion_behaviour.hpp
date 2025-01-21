// include/beman/execution26/detail/completion_behaviour.hpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_BEHAVIOUR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_BEHAVIOUR

#include <cstdint>

namespace beman::execution26 {
enum class completion_behaviour : std::uint8_t { inline_completion, synchronous, asynchronous, unknown };
} // namespace beman::execution26

#endif
