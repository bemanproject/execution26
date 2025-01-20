// examples/doc-just.cpp                                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/execution.hpp>
#include <cassert>
#include <string>
namespace ex = beman::execution26;
using namespace std::string_literals;

int main() {
    auto result = ex::sync_wait(ex::just(17, "hello"s, true));
    assert(result);
    assert(*result == std::tuple(17, "hello"s, true));
}
