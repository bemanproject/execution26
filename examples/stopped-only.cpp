// examples/set_stopped.cpp                                           -*-C++-*-
// ----------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

#include <beman/execution26/execution.hpp>

namespace ex = beman::execution26;

// ----------------------------------------------------------------------------

int main()
{
    ex::sync_wait(ex::just_stopped());
}
