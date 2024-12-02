// include/beman/execution26/detail/intrusive_queue.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_INTRUSIVE_QUEUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_INTRUSIVE_QUEUE

#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <auto>
struct intrusive_queue;

template <typename T, T* T::* next>
struct intrusive_queue<next> {
    T* head{};
    T* tail{};

    intrusive_queue() = default;
    intrusive_queue(intrusive_queue&& other)
        : head(::std::exchange(other.head, nullptr))
        , tail(::std::exchange(other.tail, nullptr))
    {
    }
    auto push(T* n) -> void {
        if (this->head) {
            std::exchange(this->tail, n)->*next = n;
        } else {
            this->head = this->tail = n;
        }
    }
    auto pop() -> T* { return std::exchange(this->head, this->head->*next); }
    auto empty() const -> bool { return this->head == nullptr; }
};
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
