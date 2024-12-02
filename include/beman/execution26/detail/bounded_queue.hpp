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
#include <memory>
#include <mutex>
#include <optional>
#include <condition_variable>
#include <type_traits>
#include <utility>
#include <cstddef>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
template <typename T, typename = ::std::allocator<T>>
class bounded_queue;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
class beman::execution26::bounded_queue : ::beman::execution26::detail::immovable {
  public:
    using value_type     = T;
    using allocator_type = Allocator;

    struct push_sender {
        struct state_base {
            ::std::remove_cvref_t<T> value;
            bounded_queue&           queue;
            state_base*              next{};

            template <typename V>
            state_base(V&& value, bounded_queue& queue) : value(::std::forward<V>(value)), queue(queue) {}
            virtual auto complete() -> void                                    = 0;
            virtual auto complete(::beman::execution26::conqueue_errc) -> void = 0;
        };
        template <typename Receiver>
        struct state : state_base {
            using operation_state_concept = ::beman::execution26::operation_state_t;

            ::std::remove_cvref_t<Receiver> receiver;

            template <typename R>
            state(bounded_queue& queue, T&& value, R&& receiver)
                : state_base(::std::move(value), queue), receiver(::std::forward<R>(receiver)) {}

            auto start() & noexcept {
                if (this->queue.start_push(*this))
                    this->complete();
            }
            auto complete() -> void override { ::beman::execution26::set_value(::std::move(this->receiver)); }
            auto complete(::beman::execution26::conqueue_errc error) -> void override {
                ::beman::execution26::set_error(::std::move(this->receiver), error);
            }
        };

        using sender_concept = ::beman::execution26::sender_t;
        using completion_signatures =
            ::beman::execution26::completion_signatures<::beman::execution26::set_value_t(),
                                                        ::beman::execution26::set_error_t(::beman::execution26::conqueue_errc),
                                                        ::beman::execution26::set_stopped_t()>;

        bounded_queue&           queue;
        ::std::remove_cvref_t<T> value;
        template <::beman::execution26::receiver Receiver>
        auto connect(Receiver&& receiver) && -> state<Receiver> {
            static_assert(::beman::execution26::operation_state<state<Receiver>>);
            return state<Receiver>(queue, ::std::move(value), ::std::forward<Receiver>(receiver));
        }
    };
    static_assert(::beman::execution26::sender<push_sender>);
    static_assert(::beman::execution26::sender_in<push_sender>);

    struct pop_sender {
        struct state_base {
            bounded_queue&           queue;
            state_base*              next{};

            state_base(bounded_queue& queue) : queue(queue) {}
            virtual auto complete(T) -> void                                    = 0;
            virtual auto complete(::beman::execution26::conqueue_errc) -> void = 0;
        };
        template <typename Receiver>
        struct state : state_base {
            using operation_state_concept = ::beman::execution26::operation_state_t;

            ::std::remove_cvref_t<Receiver> receiver;

            template <typename R>
            state(bounded_queue& queue, R&& receiver)
                : state_base(queue), receiver(::std::forward<R>(receiver)) {}

            auto start() & noexcept {
                this->queue.start_pop(*this);
            }
            auto complete(T val) -> void override { ::beman::execution26::set_value(::std::move(this->receiver), ::std::move(val)); }
            auto complete(::beman::execution26::conqueue_errc error) -> void override {
                ::beman::execution26::set_error(::std::move(this->receiver), error);
            }
        };

        using sender_concept = ::beman::execution26::sender_t;
        using completion_signatures =
            ::beman::execution26::completion_signatures<::beman::execution26::set_value_t(T),
                                                        ::beman::execution26::set_error_t(::beman::execution26::conqueue_errc),
                                                        ::beman::execution26::set_stopped_t()>;

        bounded_queue& queue;
        template <::beman::execution26::receiver Receiver>
        auto connect(Receiver&& receiver) && -> state<Receiver> {
            static_assert(::beman::execution26::operation_state<state<Receiver>>);
            return state<Receiver>(queue, ::std::forward<Receiver>(receiver));
        }
    };
    static_assert(::beman::execution26::sender<pop_sender>);
    static_assert(::beman::execution26::sender_in<pop_sender>);

    static_assert(::std::same_as<value_type, typename Allocator::value_type>);

    explicit bounded_queue(::std::size_t max, Allocator allocator = {})
        : allocator(allocator),
          array_allocator(allocator),
          max(max),
          elements(array_allocator_traits::allocate(this->array_allocator, this->max)) {}
    ~bounded_queue() {
        for (; this->tail != this->head; ++this->tail) {
            this->destroy(this->get(this->tail));
        }
        array_allocator_traits::deallocate(this->array_allocator, this->elements, this->max);
    }

    auto is_closed() const noexcept -> bool {
        std::lock_guard cerberos(this->mutex);
        return this->closed;
    }
    auto close() noexcept -> void {
        std::unique_lock cerberus(this->mutex);
        this->closed = true;
        push_sender_queue push_queue(std::move(this->push_queue));
        pop_sender_queue pop_queue(std::move(this->pop_queue));
        cerberus.unlock();
        this->push_condition.notify_all();
        this->pop_condition.notify_all();

        while (not push_queue.empty()) {
            push_queue.pop()->complete(::beman::execution26::conqueue_errc::closed);
        }
        while (not pop_queue.empty()) {
            pop_queue.pop()->complete(::beman::execution26::conqueue_errc::closed);
        }
    }

    auto push(const T& value) -> void { this->internal_push(value); }
    auto push(T&& value) -> void { this->internal_push(::std::move(value)); }
    auto push(const T& value, ::std::error_code& ec) -> bool { return this->internal_push(value, ec); }
    auto push(T&& value, ::std::error_code& ec) -> bool { return this->internal_push(::std::move(value), ec); }
    auto try_push(const T& value, ::std::error_code& ec) -> bool { return this->internal_try_push(value, ec); }
    auto try_push(T&& value, ::std::error_code& ec) -> bool { return this->internal_try_push(::std::move(value), ec); }
    auto async_push(const T& value) -> sender auto { return internal_async_push(value); }
    auto async_push(T&& value) -> sender auto { return internal_async_push(::std::move(value)); }

    auto pop() -> T {
        ::std::unique_lock cerberus(this->mutex);
        this->pop_condition.wait(cerberus, [this] { return this->closed || this->head != this->tail; });
        if (this->closed) {
            throw ::beman::execution26::conqueue_error(::beman::execution26::conqueue_errc::closed);
        }
        element_t* element{this->get(this->tail)};
        T          rc{::std::move(element->value)};
        this->destroy(element);
        ++this->tail;
        this->push_notify(cerberus);
        return rc;
    }
    auto pop(::std::error_code& ec) -> ::std::optional<T> {
        ::std::unique_lock cerberus(this->mutex);
        this->pop_condition.wait(cerberus, [this] { return this->closed || this->head != this->tail; });
        if (this->closed) {
            ec = make_error_code(::beman::execution26::conqueue_errc::closed);
            return {};
        }
        element_t*         element{this->get(this->tail)};
        ::std::optional<T> rc{::std::move(element->value)};
        this->destroy(element);
        ++this->tail;
        this->push_notify(cerberus);
        return rc;
    }
    auto try_pop(::std::error_code& ec) -> ::std::optional<T> {
        ::std::unique_lock cerberus(this->mutex);
        if (this->closed) {
            ec = make_error_code(::beman::execution26::conqueue_errc::closed);
            return {};
        }
        if (this->head == this->tail) {
            ec = make_error_code(::beman::execution26::conqueue_errc::empty);
            return {};
        }
        element_t*         element{this->get(this->tail)};
        ::std::optional<T> rc{::std::move(element->value)};
        this->destroy(element);
        ++this->tail;
        this->push_notify(cerberus);
        return rc;
    }
    auto async_pop() -> sender auto { return pop_sender(*this); }

  private:
    using allocator_traits = ::std::allocator_traits<Allocator>;
    union element_t {
        element_t() {}
        template <typename... Args>
        element_t(allocator_type& alloc, Args&&... args) {
            allocator_traits::construct(alloc, &value, ::std::forward<Args>(args)...);
        }
        ~element_t() {}
        T value;
    };
    using array_allocator_type   = allocator_traits::template rebind_alloc<element_t>;
    using array_allocator_traits = allocator_traits::template rebind_traits<element_t>;
    using push_sender_queue      = ::beman::execution26::detail::intrusive_queue<&push_sender::state_base::next>;
    using pop_sender_queue       = ::beman::execution26::detail::intrusive_queue<&pop_sender::state_base::next>;

    template <typename... Args>
    auto construct(element_t* element, Args&&... args) {
        array_allocator_traits::construct(
            this->array_allocator, this->get(this->head), this->allocator, ::std::forward<Args>(args)...);
    }
    auto destroy(element_t* element) {
        allocator_traits::destroy(this->allocator, &element->value);
        array_allocator_traits::destroy(this->array_allocator, element);
    }
    auto get(::std::uint64_t index) noexcept -> element_t* { return this->elements + (index % this->max); }

    template <typename Arg>
    auto internal_push(Arg&& arg) -> void {
        ::beman::execution26::conqueue_errc error{};
        if (not this->internal_push(::std::forward<Arg>(arg), error)) {
            throw ::beman::execution26::conqueue_error(error);
        }
    }
    template <typename Arg>
    auto internal_push(Arg&& arg, ::std::error_code& ec) -> bool {
        ::beman::execution26::conqueue_errc error{};
        bool                                rc{this->internal_push(::std::forward<Arg>(arg), error)};
        if (not rc) {
            ec = make_error_code(error);
        }
        return rc;
    }
    template <typename Arg>
    auto internal_push(Arg&& arg, ::beman::execution26::conqueue_errc& error) -> bool {
        ::std::unique_lock cerberus(this->mutex);
        this->push_condition.wait(cerberus, [this] { return this->closed || this->head - this->tail < this->max; });
        if (this->closed) {
            error = ::beman::execution26::conqueue_errc::closed;
            return false;
        }
        this->construct(this->get(this->head), ::std::forward<Arg>(arg));
        ++this->head;
        this->pop_notify(cerberus);
        return true;
    }
    template <typename Arg>
    auto internal_try_push(Arg&& arg, ::std::error_code& ec) -> bool {
        ::std::unique_lock cerberus(this->mutex);
        if (this->closed) {
            ec = make_error_code(::beman::execution26::conqueue_errc::closed);
            return false;
        }
        if (this->head - this->tail == this->max) {
            ec = make_error_code(::beman::execution26::conqueue_errc::full);
            return false;
        }
        this->construct(this->get(this->head), ::std::forward<Arg>(arg));
        ++this->head;
        this->pop_notify(cerberus);
        return true;
    }
    auto pop_notify(auto& cerberus) {
        if (not this->pop_queue.empty())
        {
            element_t* element{this->get(this->tail)};
            ::std::remove_cvref_t<T> val{std::move(element->value)};
            this->destroy(element);
            ++this->tail;
            auto state{this->pop_queue.pop()};
            this->push_notify(cerberus);
            state->complete(std::move(val));
        }
        else
        {
            cerberus.unlock();
            this->pop_condition.notify_one();
        }
    }
    auto push_notify(auto& cerberus) {
        if (not this->push_queue.empty())
        {
            cerberus.unlock();
            this->push_queue.pop()->complete();
        }
        else
        {
            cerberus.unlock();
            this->push_condition.notify_one();
        }
    }
    template <typename TT>
    auto internal_async_push(TT&& value) -> sender auto {
        return push_sender{*this, ::std::forward<TT>(value)};
    }
    auto start_push(push_sender::state_base& s) -> bool {
        std::unique_lock cerberus(this->mutex);
        if (this->head - this->tail < this->max) {
            this->construct(this->get(this->head), ::std::move(s.value));
            ++this->head;
            this->pop_notify(cerberus);
            return true;
        } else {
            this->push_queue.push(&s);
            return false;
        }
    }
    auto start_pop(pop_sender::state_base& s) -> bool {
        std::unique_lock cerberus(this->mutex);
        if (this->head != this->tail) {
            element_t* element(this->get(tail));
            std::remove_cvref_t<T> val(std::move(element->value));
            ++this->tail;
            this->destroy(element);
            this->push_notify(cerberus);
            s.complete(std::move(val));
            return true;
        } else {
            this->pop_queue.push(&s);
            return false;
        }
    }

    Allocator                 allocator;
    array_allocator_type      array_allocator;
    mutable ::std::mutex      mutex;
    push_sender_queue         push_queue;
    ::std::condition_variable push_condition;
    pop_sender_queue          pop_queue;
    ::std::condition_variable pop_condition;
    ::std::size_t             max;
    element_t*                elements;
    ::std::uint64_t           head{}; // the next element to push to
    ::std::uint64_t           tail{}; // the next element to push from
    bool                      closed{};
};

// ----------------------------------------------------------------------------

#endif
