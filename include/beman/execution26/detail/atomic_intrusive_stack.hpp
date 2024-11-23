// include/beman/execution26/detail/atomic_intrusive_stack.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_ATOMIC_INTRUSIVE_STACK
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_ATOMIC_INTRUSIVE_STACK

#include <beman/execution26/detail/intrusive_queue.hpp>

#include <atomic>
#include <cassert>
#include <optional>

namespace beman::execution26::detail {

template <auto Next>
class atomic_intrusive_stack;

//! @brief  This data structure is an intrusive stack that can be used in a lock-free manner.
//!
//! The stack is implemented as a singly linked list where the head is an atomic pointer to the first item in
//! the stack.
//! try_push() is a lock-free operation that tries to push an item to the stack. If the stack is empty, it
//! returns nullptr and the item is pushed to the stack.
//! This stack has a closed state, which is indicated by the head pointing to the stack itself. In this state,
//! try_push() returns std::nullopt and the stack is not modified.
//! pop_all_and_shutdown() is a lock-free operation that pops all items from the stack and returns them in a
//! queue. If the stack is empty, it returns an empty queue.
//!
//! We use this stack in the split implementation to store the listeners that are waiting for the operation to
//! complete.
//!
//! @tparam Item  The type of the item in the stack.
//! @tparam Next  The pointer to the next item in the stack.
template <class Item, Item* Item::*Next>
class atomic_intrusive_stack<Next> {
  public:
    ~atomic_intrusive_stack() { assert(!head_ || head_ == this); }

    //! @brief  Tries to push an item to the stack.
    //!
    //! @param item  The item to push to the stack.
    //!
    //! @return  If the stack is empty, returns nullptr and pushes the item to the stack.
    //!          If the stack is in the closed state, returns std::nullopt.
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

    //! @brief  Tests if the stack is empty and not in the closed state.
    auto empty_and_not_shutdown() const noexcept -> bool { return head_.load() == nullptr; }

    //! @brief  Pops all items from the stack and returns them in a queue and sets the stack to the closed state.
    //!
    //! @return  If the stack is empty, returns an empty queue.
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