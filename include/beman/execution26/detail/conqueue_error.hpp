// include/beman/execution26/detail/conqueue_error.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONQUEUE_ERROR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONQUEUE_ERROR

#include <beman/execution26/detail/conqueue_errc.hpp>
#include <system_error>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
class conqueue_error : public ::std::system_error {
  public:
    explicit conqueue_error(::beman::execution26::conqueue_errc ec)
        : std::system_error(::beman::execution26::make_error_code(ec),
                            ::beman::execution26::conqueue_category().message(static_cast<int>(ec))) {}
};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
