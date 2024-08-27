// include/beman/execution26/detail/meta_unique.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_UNIQUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_UNIQUE

#include <beman/execution26/detail/meta_contains.hpp>
#include <beman/execution26/detail/meta_prepend.hpp>
#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta::detail
{
    template <template <typename...> class List, typename... T>
    struct unique;

    template <template <typename...> class List>
    struct unique<List>
    {
        using type = List<>;
    };
    template <template <typename...> class List, typename T>
    struct unique<List, T>
    {
        using type = List<T>;
    };

    template <template <typename...> class List, typename T, typename... S>
    struct unique<List, T, S...>
    {
        using tail = ::beman::execution26::detail::meta::detail::unique<List, S...>::type;
        using type = ::std::conditional_t<
            ::beman::execution26::detail::meta::contains<T, S...>,
            tail,
            ::beman::execution26::detail::meta::prepend<List, T, tail>
            >;
    };
}

namespace beman::execution26::detail::meta
{
    template <template <typename...> class List, typename... T>
    using unique
        = ::beman::execution26::detail::meta::detail::unique<List, T...>::type;
}

// ----------------------------------------------------------------------------

#endif
