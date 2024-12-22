// examples/doc-just_error.cpp                                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/execution.hpp>
#include <system_error>
#include <cassert>
namespace ex = beman::execution26;

namespace {
void use(auto&&...) {}
} // namespace

int main() {
    bool had_error{false};
    auto result = ex::sync_wait(ex::just_error(std::error_code(17, std::system_category())) |
                                ex::upon_error([&](std::error_code ec) {
                                    use(ec);
                                    assert(ec.value() == 17);
                                    had_error = true;
                                }));
    use(result, had_error);
    assert(result);
    assert(had_error);
}
