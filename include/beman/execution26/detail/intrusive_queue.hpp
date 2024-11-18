// include/beman/execution26/detail/intrusive_queue.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_INTRUSIVE_QUEUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_INTRUSIVE_QUEUE

#include <cassert>
#include <utility>

namespace beman::execution26::detail {

template <auto Next>
class intrusive_queue;

template <class Item, Item* Item::* Next>
class intrusive_queue<Next> {
  public:
    ~intrusive_queue() { assert(!head_); }

    auto push(Item* item) noexcept -> void {
        if (last_) {
            assert(head_);
            last_->*Next = item;
        } else {
            assert(!head_ && !last_);
            head_ = item;
            last_ = item;
        }
    }

    auto pop() noexcept -> Item* {
        if (head_) {
            auto item = head_;
            head_     = std::exchange(item->*Next, nullptr);
            if (!head_) {
                last_ = nullptr;
            }
            return item;
        }
        assert(!last_);
        return nullptr;
    }

    auto empty() const noexcept -> bool { return !head_; }

  private:
    Item* head_{nullptr};
    Item* last_{nullptr};
};

} // namespace beman::execution26::detail

#endif