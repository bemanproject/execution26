// include/beman/execution26/detail/bounded_queue.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_BOUNDED_QUEUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_BOUNDED_QUEUE

#include <beman/execution26/detail/immovable.hpp>
#include <beman/execution26/detail/intrusive_queue.hpp>
#include <beman/execution26/detail/conqueue_errc.hpp>
#include <beman/execution26/detail/conqueue_error.hpp>
#include <beman/execution26/execution.hpp>
#include <concepts>
#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <condition_variable>
#include <type_traits>
#include <utility>
#include <variant>
#include <cstddef>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
//! @brief This class templates provides a thread-safe, bounded queue
template <typename T, typename = ::std::allocator<T>>
class bounded_queue;
} // namespace beman::execution26

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
class beman::execution26::bounded_queue : ::beman::execution26::detail::immovable {
  public:
    using value_type       = ::std::remove_cvref_t<T>;
    using allocator_type   = Allocator;
    using allocator_traits = ::std::allocator_traits<allocator_type>;
    class push_sender;
    class pop_sender;

    static_assert(::std::same_as<value_type, typename Allocator::value_type>);

    explicit bounded_queue(::std::size_t max, Allocator allocator = {});
    ~bounded_queue();

    auto is_closed() const noexcept -> bool;
    auto close() noexcept -> void;

    auto push(const T& value) -> void;
    auto push(T&& value) -> void;
    auto push(const T& value, ::std::error_code& ec) -> bool;
    auto push(T&& value, ::std::error_code& ec) -> bool;
    auto try_push(const T& value, ::std::error_code& ec) -> bool;
    auto try_push(T&& value, ::std::error_code& ec) -> bool;
    auto async_push(const T& value) -> push_sender;
    auto async_push(T&& value) -> push_sender;

    auto pop() -> value_type;
    auto pop(::std::error_code& ec) -> ::std::optional<value_type>;
    auto try_pop(::std::error_code& ec) -> ::std::optional<value_type>;
    auto async_pop() -> pop_sender;

  private:
    struct push_base {
        value_type value;
        push_base* next{};

        template <typename V>
        push_base(V&& value) : value(::std::forward<V>(value)) {}
        virtual auto complete() -> void                                    = 0;
        virtual auto complete(::beman::execution26::conqueue_errc) -> void = 0;
        virtual auto complete(::std::exception_ptr) -> void                = 0;
    };
    struct pop_base {
        pop_base* next{};

        pop_base()                                                         = default;
        virtual auto complete(value_type) -> void                          = 0;
        virtual auto complete(::beman::execution26::conqueue_errc) -> void = 0;
        virtual auto complete(::std::exception_ptr) -> void                = 0;
    };

    struct blocking_push_state;
    struct blocking_pop_state;

    union element_t {
        element_t() {}
        template <typename... Args>
        element_t(allocator_type& alloc, Args&&... args) {
            allocator_traits::construct(alloc, &value, ::std::forward<Args>(args)...);
        }
        ~element_t() {}
        value_type value;
    };

    using array_allocator_type   = allocator_traits::template rebind_alloc<element_t>;
    using array_allocator_traits = allocator_traits::template rebind_traits<element_t>;
    using push_sender_queue      = ::beman::execution26::detail::intrusive_queue<&push_base::next>;
    using pop_sender_queue       = ::beman::execution26::detail::intrusive_queue<&pop_base::next>;

    template <typename... Args>
    auto construct(element_t* element, Args&&... args) -> void;
    auto destroy(element_t* element) -> void;
    auto get(::std::uint64_t index) noexcept -> element_t*;
    template <typename Lock>
    auto has_space(Lock&) noexcept -> bool;
    template <typename Lock, typename V>
    auto push_value(Lock&, V&&) -> void;
    template <typename Lock>
    auto pop_value(Lock&) -> value_type;

    template <typename Arg>
    auto internal_push(Arg&& arg, ::beman::execution26::conqueue_errc& error) -> bool;
    template <typename Arg>
    auto internal_try_push(Arg&& arg, ::std::error_code& ec) -> bool;
    template <typename HandleResult>
    auto internal_pop(HandleResult);

    auto start_push(push_base& s) -> void;
    auto start_pop(pop_base& s) -> void;

    auto pop_notify(auto& cerberus) -> void;
    auto push_notify(auto& cerberus) -> void;

    allocator_type       allocator;
    array_allocator_type array_allocator;
    mutable ::std::mutex mutex;
    push_sender_queue    push_queue;
    pop_sender_queue     pop_queue;
    ::std::size_t        max;
    element_t*           elements;
    ::std::uint64_t      head{}; // the next element to push to
    ::std::uint64_t      tail{}; // the next element to push from
    bool                 closed{};
};

template <typename T, typename Allocator>
struct beman::execution26::bounded_queue<T, Allocator>::blocking_push_state : push_base {
    bool                                                                                        ready{false};
    ::std::variant<::std::monostate, ::beman::execution26::conqueue_errc, ::std::exception_ptr> result;
    ::std::condition_variable                                                                   condition;
    bounded_queue&                                                                              queue;

    template <typename Arg>
    blocking_push_state(bounded_queue& queue, Arg&& arg) : push_base(::std::forward<Arg>(arg)), queue(queue) {}
    auto complete() -> void override {
        ::std::lock_guard cerberus(queue.mutex);
        {
            this->ready = true;
        }
        this->condition.notify_one();
    }
    auto complete(::beman::execution26::conqueue_errc err) -> void override {
        result = err;
        this->complete();
    }
    auto complete(::std::exception_ptr ex) -> void override {
        result = std::move(ex);
        this->complete();
    }
};
template <typename T, typename Allocator>
struct beman::execution26::bounded_queue<T, Allocator>::blocking_pop_state : pop_base {
    bounded_queue& queue;
    bool           ready{false};
    ::std::variant<::std::monostate, value_type, ::beman::execution26::conqueue_errc, ::std::exception_ptr> result{};
    ::std::condition_variable condition{};

    blocking_pop_state(bounded_queue& queue) : queue(queue) {}
    auto complete(value_type value) -> void override {
        {
            ::std::lock_guard cerberus(queue.mutex);
            this->result = ::std::move(value);
            this->ready  = true;
        }
        this->condition.notify_one();
    }
    auto complete(::beman::execution26::conqueue_errc err) -> void override {
        {
            ::std::lock_guard cerberus(queue.mutex);
            result      = err;
            this->ready = true;
        }
        this->condition.notify_one();
    }
    auto complete(::std::exception_ptr ex) -> void override {
        {
            ::std::lock_guard cerberus(queue.mutex);
            result      = std::move(ex);
            this->ready = true;
        }
        this->condition.notify_one();
    }
};

template <typename T, typename Allocator>
class beman::execution26::bounded_queue<T, Allocator>::push_sender {
  public:
    template <typename Receiver>
    struct state : push_base {
        using operation_state_concept = ::beman::execution26::operation_state_t;

        bounded_queue&                  queue;
        ::std::remove_cvref_t<Receiver> receiver;

        template <typename R>
        state(bounded_queue& queue, T&& value, R&& receiver)
            : push_base(::std::move(value)), queue(queue), receiver(::std::forward<R>(receiver)) {}

        auto start() & noexcept { this->queue.start_push(*this); }
        auto complete() -> void override { ::beman::execution26::set_value(::std::move(this->receiver)); }
        auto complete(::beman::execution26::conqueue_errc error) -> void override {
            ::beman::execution26::set_error(::std::move(this->receiver), error);
        }
        auto complete(::std::exception_ptr ex) -> void override {
            ::beman::execution26::set_error(::std::move(this->receiver), std::move(ex));
        }
    };

    using sender_concept = ::beman::execution26::sender_t;
    //-dk:TODO remove exception_ptr if move-ctor can't throw
    using completion_signatures =
        ::beman::execution26::completion_signatures<::beman::execution26::set_value_t(),
                                                    ::beman::execution26::set_error_t(
                                                        ::beman::execution26::conqueue_errc),
                                                    ::beman::execution26::set_error_t(::std::exception_ptr),
                                                    ::beman::execution26::set_stopped_t()>;

    bounded_queue&           queue;
    ::std::remove_cvref_t<T> value;
    template <typename V>
    push_sender(bounded_queue& queue, V&& value) : queue(queue), value(::std::forward<V>(value)) {}
    template <::beman::execution26::receiver Receiver>
    auto connect(Receiver&& receiver) && -> state<Receiver> {
        return state<Receiver>(queue, ::std::move(value), ::std::forward<Receiver>(receiver));
    }
};

template <typename T, typename Allocator>
class beman::execution26::bounded_queue<T, Allocator>::pop_sender {
  public:
    template <typename Receiver>
    struct state : pop_base {
        using operation_state_concept = ::beman::execution26::operation_state_t;

        bounded_queue&                  queue;
        ::std::remove_cvref_t<Receiver> receiver;

        template <typename R>
        state(bounded_queue& queue, R&& receiver) : pop_base(), queue(queue), receiver(::std::forward<R>(receiver)) {}

        auto start() & noexcept { this->queue.start_pop(*this); }
        auto complete(T val) -> void override {
            ::beman::execution26::set_value(::std::move(this->receiver), ::std::move(val));
        }
        auto complete(::beman::execution26::conqueue_errc error) -> void override {
            ::beman::execution26::set_error(::std::move(this->receiver), error);
        }
        auto complete(::std::exception_ptr ex) -> void override {
            ::beman::execution26::set_error(::std::move(this->receiver), ::std::move(ex));
        }
    };

    using sender_concept        = ::beman::execution26::sender_t;
    using completion_signatures = ::beman::execution26::completion_signatures<::beman::execution26::set_value_t(T),
                                                                              ::beman::execution26::set_error_t(
                                                                                  ::beman::execution26::conqueue_errc),
                                                                              ::beman::execution26::set_stopped_t()>;

    bounded_queue& queue;
    template <::beman::execution26::receiver Receiver>
    auto connect(Receiver&& receiver) && -> state<Receiver> {
        return state<Receiver>(queue, ::std::forward<Receiver>(receiver));
    }
};

template <typename T, typename Allocator>
beman::execution26::bounded_queue<T, Allocator>::bounded_queue(::std::size_t max, Allocator allocator)
    : allocator(allocator),
      array_allocator(allocator),
      max(max),
      elements(array_allocator_traits::allocate(this->array_allocator, this->max)) {}

template <typename T, typename Allocator>
beman::execution26::bounded_queue<T, Allocator>::~bounded_queue() {
    for (; this->tail != this->head; ++this->tail) {
        this->destroy(this->get(this->tail));
    }
    array_allocator_traits::deallocate(this->array_allocator, this->elements, this->max);
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::is_closed() const noexcept -> bool {
    std::lock_guard cerberos(this->mutex);
    return this->closed;
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::close() noexcept -> void {
    std::unique_lock cerberus(this->mutex);
    this->closed = true;
    push_sender_queue push_queue(std::move(this->push_queue));
    pop_sender_queue  pop_queue(std::move(this->pop_queue));
    assert(cerberus.owns_lock());
    cerberus.unlock();

    while (not push_queue.empty()) {
        push_queue.pop()->complete(::beman::execution26::conqueue_errc::closed);
    }
    while (not pop_queue.empty()) {
        pop_queue.pop()->complete(::beman::execution26::conqueue_errc::closed);
    }
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::push(const T& value) -> void {
    ::beman::execution26::conqueue_errc error{};
    if (not this->internal_push(value, error)) {
        throw ::beman::execution26::conqueue_error(error);
    }
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::push(T&& value) -> void {
    ::beman::execution26::conqueue_errc error{};
    if (not this->internal_push(::std::move(value), error)) {
        throw ::beman::execution26::conqueue_error(error);
    }
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::push(const T& value, ::std::error_code& ec) -> bool {
    ::beman::execution26::conqueue_errc error{};
    if (not this->internal_push(value, error)) {
        ec = make_error_code(error);
        return false;
    }
    return true;
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::push(T&& value, ::std::error_code& ec) -> bool {
    ::beman::execution26::conqueue_errc error{};
    if (not this->internal_push(value, error)) {
        ec = make_error_code(error);
        return false;
    }
    return true;
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::try_push(const T& value, ::std::error_code& ec) -> bool {
    return this->internal_try_push(value, ec);
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::try_push(T&& value, ::std::error_code& ec) -> bool {
    return this->internal_try_push(::std::move(value), ec);
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::async_push(const T& value) -> push_sender {
    return push_sender(*this, value);
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::async_push(T&& value) -> push_sender {
    return push_sender(*this, ::std::move(value));
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::pop() -> value_type {
    return this->internal_pop([](auto&& variant) {
        switch (variant.index()) {
        default:
            throw ::std::runtime_error("unknown result in bounded_queue");
        case 1:
            return ::std::move(::std::get<1>(variant));
        case 2:
            throw ::beman::execution26::conqueue_error(::std::get<2>(variant));
        case 3:
            ::std::rethrow_exception(::std::get<3>(variant));
        }
    });
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::pop(::std::error_code& ec) -> ::std::optional<value_type> {
    return this->internal_pop([&ec](auto&& variant) {
        switch (variant.index()) {
        default:
            throw ::std::runtime_error("unknown result in bounded_queue");
        case 1:
            return ::std::optional<value_type>(::std::move(::std::get<1>(variant)));
        case 2:
            ec = make_error_code(::std::get<2>(variant));
            return ::std::optional<value_type>();
        case 3:
            ::std::rethrow_exception(::std::get<3>(variant));
        }
    });
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::try_pop(::std::error_code& ec) -> ::std::optional<value_type> {
    ::std::unique_lock cerberus(this->mutex);
    if (this->closed) {
        ec = make_error_code(::beman::execution26::conqueue_errc::closed);
        return {};
    }
    if (this->head == this->tail) {
        ec = make_error_code(::beman::execution26::conqueue_errc::empty);
        return {};
    }
    return this->pop_value(cerberus);
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::async_pop() -> pop_sender {
    return pop_sender(*this);
}

template <typename T, typename Allocator>
template <typename... Args>
auto beman::execution26::bounded_queue<T, Allocator>::construct(element_t* element, Args&&... args) -> void {
    array_allocator_traits::construct(
        this->array_allocator, this->get(this->head), this->allocator, ::std::forward<Args>(args)...);
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::destroy(element_t* element) -> void {
    allocator_traits::destroy(this->allocator, &element->value);
    array_allocator_traits::destroy(this->array_allocator, element);
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::get(::std::uint64_t index) noexcept -> element_t* {
    return this->elements + (index % this->max);
}

template <typename T, typename Allocator>
template <typename HandleResult>
auto beman::execution26::bounded_queue<T, Allocator>::internal_pop(HandleResult handle_result) {
    blocking_pop_state s(*this);
    this->start_pop(s);
    {
        ::std::unique_lock cerberus(this->mutex);
        s.condition.wait(cerberus, [&ready = s.ready] { return ready; });
    }
    return handle_result(::std::move(s.result));
}

template <typename T, typename Allocator>
template <typename Arg>
auto beman::execution26::bounded_queue<T, Allocator>::internal_push(Arg&&                                arg,
                                                                    ::beman::execution26::conqueue_errc& error)
    -> bool {
    blocking_push_state s(*this, ::std::forward<Arg>(arg));
    this->start_push(s);
    {
        ::std::unique_lock cerberus(this->mutex);
        s.condition.wait(cerberus, [&ready = s.ready] { return ready; });
    }
    switch (s.result.index()) {
    case 0:
        return true;
    case 1:
        error = ::std::get<1>(s.result);
        break;
    case 2:
        std::rethrow_exception(::std::get<2>(s.result));
    }
    return false;
}

template <typename T, typename Allocator>
template <typename Arg>
auto beman::execution26::bounded_queue<T, Allocator>::internal_try_push(Arg&& arg, ::std::error_code& ec) -> bool {
    ::std::unique_lock cerberus(this->mutex);
    if (this->closed) {
        ec = make_error_code(::beman::execution26::conqueue_errc::closed);
        return false;
    }
    if (not this->has_space(cerberus)) {
        ec = make_error_code(::beman::execution26::conqueue_errc::full);
        return false;
    }
    this->push_value(cerberus, ::std::forward<Arg>(arg));
    return true;
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::pop_notify(auto& cerberus) -> void {
    assert(cerberus.owns_lock());
    if (not this->pop_queue.empty()) {
        auto state{this->pop_queue.pop()};
        state->complete(this->pop_value(cerberus));
    } else {
        cerberus.unlock();
    }
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::push_notify(auto& cerberus) -> void {
    assert(cerberus.owns_lock());
    if (not this->push_queue.empty()) {
        push_base* push(this->push_queue.pop());
        try {
            this->push_value(cerberus, ::std::move(push->value));
            assert(not cerberus.owns_lock());
        } catch (...) {
            cerberus.unlock();
            push->complete(::std::current_exception());
            assert(not cerberus.owns_lock());
            return;
        }
        push->complete();
    } else {
        cerberus.unlock();
    }
    assert(not cerberus.owns_lock());
}

template <typename T, typename Allocator>
template <typename Lock>
auto beman::execution26::bounded_queue<T, Allocator>::has_space(Lock&) noexcept -> bool {
    return this->head - this->tail < this->max;
}

template <typename T, typename Allocator>
template <typename Lock, typename V>
auto beman::execution26::bounded_queue<T, Allocator>::push_value(Lock& cerberus, V&& value) -> void {

    this->construct(this->get(this->head), ::std::forward<V>(value));
    ++this->head;
    assert(cerberus.owns_lock());
    this->pop_notify(cerberus);
    assert(not cerberus.owns_lock());
}

template <typename T, typename Allocator>
template <typename Lock>
auto beman::execution26::bounded_queue<T, Allocator>::pop_value(Lock& cerberus) -> value_type {
    element_t* element(this->get(tail));
    value_type val(std::move(element->value));
    ++this->tail;
    this->destroy(element);
    this->push_notify(cerberus);
    return val;
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::start_push(push_base& s) -> void {
    std::unique_lock cerberus(this->mutex);
    if (this->closed) {
        cerberus.unlock();
        s.complete(::beman::execution26::conqueue_errc::closed);
    } else if (this->has_space(cerberus)) {
        try {
            this->push_value(cerberus, std::move(s.value));
            s.complete();
        } catch (...) {
            assert(cerberus.owns_lock());
            cerberus.unlock();
            s.complete(std::current_exception());
        }
    } else {
        this->push_queue.push(&s);
    }
}

template <typename T, typename Allocator>
auto beman::execution26::bounded_queue<T, Allocator>::start_pop(pop_base& s) -> void {
    std::unique_lock cerberus(this->mutex);
    if (this->closed) {
        cerberus.unlock();
        s.complete(::beman::execution26::conqueue_errc::closed);
    } else if (this->head != this->tail) {
        s.complete(this->pop_value(cerberus));
    } else {
        this->pop_queue.push(&s);
    }
}

// ----------------------------------------------------------------------------

#endif