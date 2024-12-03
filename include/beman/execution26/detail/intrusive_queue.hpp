// include/beman/execution26/detail/intrusive_queue.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_INTRUSIVE_QUEUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_INTRUSIVE_QUEUE

#include <cassert>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <auto>
struct intrusive_queue;

//! @brief This class template implements a non-thread-safe, intrusive queue.
//! @headerfile beman/execution26/detail/intrusive_queue.hpp <beman/execution26/detail/intrusive_queue.hpp>
//! @details
//! The template argument is a pointer-to-member for some class which
//! is itself a pointer to the type. The member is used to form a singly
//! linked list.
//!
//! <h4>Usage</h4>
//! <pre>
//! intrusive_queue<&Type::next>
//! </pre>
//!
//! <h4>Example</h4>
//! <pre>
//! #include <beman/execution26/detail/intrusive_list.hpp>
//! #include <cassert>
//! namespace detail = beman::execution26::detail;
//! struct node {
//!     int   value{};
//!     node* link{};
//! };
//! int main() {
//!     detail::intrusive<&node::link> q;
//!     assert(q.empty());
//!     node ns[]{ { 1 }, { 2 }, { 3 } };
//!     for (auto& n: ns) { q.push(&n); }
//!     assert(!q.empty());
//!     assert(q.pop()->value == 1);
//!     assert(q.pop()->value == 2);
//!     assert(q.pop()->value == 3);
//! }
//! </pre>
template <typename T, T* T::* next>
struct intrusive_queue<next> {
    T* head{};
    T* tail{};

    //! @brief The default constructor create an empty queue
    constexpr intrusive_queue() noexcept = default;
    //! @brief The move constructor transfers the content of the parameter to the new queue
    //! @details
    //! The original queue `other` will be empty after this operation.
    intrusive_queue(intrusive_queue&& other) noexcept
        : head(::std::exchange(other.head, nullptr)), tail(::std::exchange(other.tail, nullptr)) {}
    //! @brief Push the node `n` to the end of the queue
    //! @details
    //! The "next" pointer of the new node is set to `nullptr`.
    auto push(T* n) noexcept -> void {
        n->*next = nullptr;
        if (this->head) {
            std::exchange(this->tail, n)->*next = n;
        } else {
            this->head = this->tail = n;
        }
    }
    //! @brief Remove the start node of the queue and return it
    //! @pre The queue is not empty
    auto pop() noexcept -> T* {
        assert(not this->empty());
        return std::exchange(this->head, this->head->*next);
    }
    //! @brief Test whether the queue is empty
    auto empty() const noexcept -> bool { return this->head == nullptr; }
};
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif
