// include/beman/execution26/detail/common.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMMON
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMMON

// ----------------------------------------------------------------------------

#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wmissing-braces"

// ----------------------------------------------------------------------------

#if defined(__cpp_deleted_function)
#    define BEMAN_EXECUTION26_DELETE(msg) delete(msg)
#else
#    define BEMAN_EXECUTION26_DELETE(msg) delete
#endif

// ----------------------------------------------------------------------------

#endif
