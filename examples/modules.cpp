// examples/modules.cpp                                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// import std;
#include <iostream>
#if __cpp_modules < 201907L
#include <beman/execution26/execution.hpp>
#else
import beman.execution26;
#endif

namespace ex = beman::execution26;

int main() {}
