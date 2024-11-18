// include/beman/execution26/detail/atomic_intrusive_queue.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_ATOMIC_INTRUSIVE_QUEUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_ATOMIC_INTRUSIVE_QUEUE

#include <beman/execution26/detail/intrusive_queue.hpp>

#include <atomic>
#include <cassert>
#include <optional>

namespace beman::execution26::detail {

template <auto Next>
class atomic_intrusive_queue;

template <class Item, Item* Item::*Next>
class atomic_intrusive_queue<Next> {
  public:
    ~atomic_intrusive_queue() { assert(!head_ || head_ == this); }

    auto try_push(Item* item) noexcept -> std::optional<void*> {
        void* ptr = head_.load();
        if (ptr == this) {
            return std::nullopt;
        }
        item->*Next = static_cast<Item*>(ptr);
        while (!head_.compare_exchange_weak(ptr, item)) {
            if (ptr == this) {
                return std::nullopt;
            }
            item->*Next = static_cast<Item*>(ptr);
        }
        return ptr;
    }

    auto empty_and_not_shutdown() const noexcept -> bool { return head_.load() == nullptr; }

    auto pop_all_and_shutdown() noexcept -> ::beman::execution26::detail::intrusive_queue<Next> {
        auto  queue = ::beman::execution26::detail::intrusive_queue<Next>{};
        void* ptr   = head_.exchange(this);
        if (ptr == this) {
            return queue;
        }
        auto item = static_cast<Item*>(ptr);
        queue.push(item);
        return queue;
    }

  private:
    ::std::atomic<void*> head_{nullptr};
};

} // namespace beman::execution26::detail

#endif