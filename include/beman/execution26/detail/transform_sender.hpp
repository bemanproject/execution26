// include/beman/execution26/detail/transform_sender.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_TRANSFORM_SENDER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_TRANSFORM_SENDER

#include <beman/execution26/detail/sender.hpp>
#include <beman/execution26/detail/default_domain.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <typename Domain, ::beman::execution26::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) &&
            requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            } &&
            (::std::same_as<::std::remove_cvref_t<Sender>,
                            std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(
                                ::std::declval<Sender>(), ::std::declval<const Env&>()...))>>)
constexpr auto transform_sender(Domain, Sender&& sender, const Env&...) noexcept -> ::beman::execution26::sender auto {
    return ::std::forward<Sender>(sender);
}

template <typename Domain, ::beman::execution26::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) &&
            requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            } &&
            (not::std::same_as<::std::remove_cvref_t<Sender>,
                               std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(
                                   ::std::declval<Sender>(), ::std::declval<const Env&>()...))>>)
constexpr auto transform_sender(Domain dom, Sender&& sender, const Env&... env) noexcept
    -> ::beman::execution26::sender decltype(auto) {
    return ::beman::execution26::detail::transform_sender(
        dom, dom.transform_sender(::std::forward<Sender>(sender), env...), env...);
}

template <typename Domain, ::beman::execution26::sender Sender, typename... Env>
    requires(not requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            }) &&
            ::std::same_as<::std::remove_cvref_t<Sender>,
                           ::std::remove_cvref_t<decltype(::beman::execution26::default_domain{}.transform_sender(
                               ::std::declval<Sender>(), ::std::declval<Env>()...))>>
constexpr auto
transform_sender(Domain, Sender&& sender, const Env&...) noexcept(noexcept(::std::forward<Sender>(sender)))
    -> ::beman::execution26::sender auto {
    return sender;
}

template <typename Domain, ::beman::execution26::sender Sender, typename... Env>
    requires(not requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            }) &&
            (not::std::same_as<::std::remove_cvref_t<Sender>,
                               ::std::remove_cvref_t<decltype(::beman::execution26::default_domain{}.transform_sender(
                                   ::std::declval<Sender>(), ::std::declval<Env>()...))>>)
constexpr auto transform_sender(Domain dom, Sender&& sender, const Env&... env) noexcept(noexcept(
    ::beman::execution26::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...)))
    -> ::beman::execution26::sender decltype(auto) {
    return ::beman::execution26::detail::transform_sender(
        dom, ::beman::execution26::default_domain{}.transform_sender(::std::forward<Sender>(sender), env...), env...);
}
} // namespace beman::execution26::detail

namespace beman::execution26 {
template <typename Domain, ::beman::execution26::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) &&
            requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            } &&
            (::std::same_as<::std::remove_cvref_t<Sender>,
                            std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(
                                ::std::declval<Sender>(), ::std::declval<const Env&>()...))>>)
constexpr auto transform_sender(Domain, Sender&& sender, const Env&...) noexcept -> ::beman::execution26::sender
    decltype(auto) {
    return ::std::forward<Sender>(sender);
}

template <typename Domain, ::beman::execution26::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) &&
            requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            } &&
            (not::std::same_as<::std::remove_cvref_t<Sender>,
                               std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(
                                   ::std::declval<Sender>(), ::std::declval<const Env&>()...))>>)
constexpr auto transform_sender(Domain dom, Sender&& sender, const Env&... env) noexcept
    -> ::beman::execution26::sender auto {
    return ::beman::execution26::detail::transform_sender(
        dom, dom.transform_sender(::std::forward<Sender>(sender), env...), env...);
}

template <typename Domain, ::beman::execution26::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) && (not requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            }) &&
            ::std::same_as<::std::remove_cvref_t<Sender>,
                           ::std::remove_cvref_t<decltype(::beman::execution26::default_domain{}.transform_sender(
                               ::std::declval<Sender>(), ::std::declval<Env>()...))>>
constexpr auto transform_sender(Domain,
                                Sender&& sender,
                                const Env&...) noexcept(noexcept(::std::forward<Sender>(sender)))
    -> ::beman::execution26::sender decltype(auto) {
    return ::std::forward<Sender>(sender);
}

template <typename Domain, ::beman::execution26::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) && (not requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            }) &&
            (not::std::same_as<::std::remove_cvref_t<Sender>,
                               ::std::remove_cvref_t<decltype(::beman::execution26::default_domain{}.transform_sender(
                                   ::std::declval<Sender>(), ::std::declval<Env>()...))>>)
constexpr auto transform_sender(Domain dom, Sender&& sender, const Env&... env) noexcept(
    noexcept(::beman::execution26::detail::transform_sender(
        dom,
        ::beman::execution26::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...),
        env...))) -> ::beman::execution26::sender decltype(auto) {
    return ::beman::execution26::detail::transform_sender(
        dom, ::beman::execution26::default_domain{}.transform_sender(::std::forward<Sender>(sender), env...), env...);
}
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
