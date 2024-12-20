// include/beman/execution26/detail/sender_decompose.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_DECOMPOSE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_DECOMPOSE

#include <tuple>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
struct sender_convert_to_any_t {
    template <typename T>
    constexpr operator T() const; // NOLINT(hicpp-explicit-conversions)
};

template <typename Tag, typename Data, typename Children>
struct sender_meta {
    using tag_type      = ::std::remove_cvref_t<Tag>;
    using data_type     = ::std::remove_cvref_t<Data>;
    using children_type = ::std::remove_cvref_t<Children>;
};

template <typename Tag, typename Data, typename Children>
struct sender_data {
    using tag_type      = ::std::remove_cvref_t<Tag>;
    using data_type     = ::std::remove_cvref_t<Data>;
    using children_type = ::std::remove_cvref_t<Children>;

    tag_type tag;
    Data&    data;
    Children children;
};

template <typename Sender>
auto get_sender_data(Sender&& sender) {
#if 0
        //-dk:TODO should use a dynamic/language approach:
        auto&& [tag, data, ... children] = sender;
        return sender_meta<decltype(tag), decltype(data), ::std::tuple<decltype(children)...>>;
#endif
    using sender_type = ::std::remove_cvref_t<Sender>;
    static constexpr ::beman::execution26::detail::sender_convert_to_any_t at{};

    if constexpr (requires {
                      sender.template get<0>();
                      sender.size();
                  })
        return [&sender]<::std::size_t... I>(::std::index_sequence<I...>) {
            return ::beman::execution26::detail::sender_data<decltype(sender.template get<0>()),
                                                             decltype(sender.template get<1>()),
                                                             decltype(::std::tie(sender.template get<2 + I>()...))>{
                sender.template get<0>(), sender.template get<1>(), ::std::tie(sender.template get<2 + I>()...)};
        }(::std::make_index_sequence<::std::decay_t<decltype(sender)>::size() - 2u>{});
    else if constexpr (requires { sender_type{at, at, at, at, at, at}; }) {
        auto&& [tag, data, c0, c1, c2, c3] = sender;
        return ::beman::execution26::detail::
            sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0, c1, c2, c3))>{
                tag, data, ::std::tie(c0, c1, c2, c3)};
    } else if constexpr (requires { sender_type{at, at, at, at, at}; }) {
        auto&& [tag, data, c0, c1, c2] = sender;
        return ::beman::execution26::detail::
            sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0, c1, c2))>{
                tag, data, ::std::tie(c0, c1, c2)};
    } else if constexpr (requires { sender_type{at, at, at, at}; }) {
        auto&& [tag, data, c0, c1] = sender;
        return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0, c1))>{
            tag, data, ::std::tie(c0, c1)};
    } else if constexpr (requires { sender_type{at, at, at}; }) {
        auto&& [tag, data, c0] = sender;
        return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0))>{
            tag, data, ::std::tie(c0)};
    } else if constexpr (requires { sender_type{at, at}; }) {
        auto&& [tag, data] = sender;
        return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), ::std::tuple<>>{
            tag, data, ::std::tuple<>()};
    } else {
        return ::beman::execution26::detail::sender_meta<void, void, void>{};
    }
}

template <typename Sender>
auto get_sender_meta(Sender&& sender) {
    using type = decltype(get_sender_data(sender));
    return sender_meta<typename type::tag_type, typename type::data_type, typename type::children_type>{};
}
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
