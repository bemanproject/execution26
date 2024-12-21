// include/beman/execution26/detail/notify.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_NOTIFY
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_NOTIFY

#include <beman/execution26/detail/make_sender.hpp>
#include <beman/execution26/detail/immovable.hpp>
#include <mutex>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
struct notify_t;
class notifier : ::beman::execution26::detail::immovable {
  public:
    auto complete() -> void {
        ::std::unique_lock kerberos(this->lock);
        this->completed = true;
        while (this->head) {
            auto* next{::std::exchange(this->head, this->head->next)};
            kerberos.unlock();
            next->complete();
            kerberos.lock();
        }
    }

  private:
    friend struct impls_for<::beman::execution26::detail::notify_t>;
    struct base : ::beman::execution26::detail::virtual_immovable {
        base*        next{};
        virtual auto complete() -> void = 0;
    };
    std::mutex lock;
    bool       completed{};
    base*      head{};

    auto add(base* b) -> bool {
        ::std::lock_guard kerberbos(this->lock);
        if (this->completed)
            return false;
        b->next = std::exchange(this->head, b);
        return true;
    }
};

struct notify_t {
    auto operator()(::beman::execution26::detail::notifier& n) const {
        return ::beman::execution26::detail::make_sender(*this, &n);
    }
};
inline constexpr ::beman::execution26::detail::notify_t notify{};

template <>
struct impls_for<::beman::execution26::detail::notify_t> : ::beman::execution26::detail::default_impls {
    template <typename Receiver>
    struct state : ::beman::execution26::detail::notifier::base {
        ::beman::execution26::detail::notifier* n;
        ::std::remove_cvref_t<Receiver>&        receiver{};
        state(::beman::execution26::detail::notifier* nn, ::std::remove_cvref_t<Receiver>& rcvr)
            : n(nn), receiver(rcvr) {}
        auto complete() -> void override { ::beman::execution26::set_value(::std::move(this->receiver)); }
    };
    static constexpr auto get_state{[]<typename Sender, typename Receiver>(Sender&& sender, Receiver&& receiver) {
        ::beman::execution26::detail::notifier* n{sender.template get<1>()};
        return state<Receiver>(n, receiver);
    }};
    static constexpr auto start{[](auto& state, auto&) noexcept -> void {
        if (not state.n->add(&state)) {
            state.complete();
        }
    }};
};

template <typename Notifier, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution26::detail::basic_sender<::beman::execution26::detail::notify_t, Notifier>,
    Env> {
    using type = ::beman::execution26::completion_signatures<::beman::execution26::set_value_t()>;
};
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
