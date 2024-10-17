// src/beman/execution26/tests/exec-let.pass.cpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/let.hpp>
#include <beman/execution26/detail/just.hpp>
#include <beman/execution26/detail/then.hpp>
#include <beman/execution26/detail/sync_wait.hpp>
#include <test/execution.hpp>
#include <cstdlib>
#include <concepts>
#include <iostream>
#include <memory_resource>
#include <span>
#include <vector>

// ----------------------------------------------------------------------------

namespace
{
    struct receiver {
        using receiver_concept = test_std::receiver_t;
        auto set_error(auto&&) && noexcept -> void {}
        auto set_stopped() && noexcept -> void {}
        auto set_value(auto&&...) && noexcept -> void {}
    };
    template <typename... Sigs>
    struct test_sender
    {
        using sender_concept = test_std::sender_t;
        using completion_signatures = test_std::completion_signatures<Sigs...>;

        struct state {
            using operation_state_concept = test_std::operation_state_t;
            auto start() & noexcept -> void {}
        };
        auto connect(auto&&) -> state { return {}; }
    };

    auto test_let_value()
    {
        auto s0{test_std::let_value(test_std::just(),
            []{ return test_std::just() | test_std::then([]{ std::cout << ">>executing let_value sender<<\n"; }); })};
        auto s1{test_std::just() | test_std::let_value([]{ return test_std::just(); })};
        static_assert(test_std::sender<decltype(s0)>);
        static_assert(test_std::sender<decltype(s1)>);

        static_assert(test_std::sender<test_sender<test_std::set_value_t()>>);
        static_assert(test_std::operation_state<test_sender<test_std::set_value_t()>::state>);
        auto s2{
            test_sender<
                test_std::set_value_t(),
                test_std::set_value_t(int),
                test_std::set_value_t(int&),
                test_std::set_error_t(int),
                test_std::set_stopped_t()
            >()
            | test_std::let_value([]<typename... A>(A&&...){
                return test_sender<
                    test_std::set_value_t(bool),
                    test_std::set_value_t(double, A...),
                    test_std::set_error_t(char)
                >();
            })
        };
        static_assert(test_std::sender<decltype(s2)>);
        using type = decltype(test_std::get_completion_signatures(s2, test_std::empty_env{}));
        static_assert(std::same_as<type, type>);
        //static_assert(std::same_as<void, type>);

        auto state2{test_std::connect(s2, receiver{})};
        test::use(state2);
        test_std::start(state2);

        auto state0{test_std::connect(s0, receiver{})};
        test::use(state0);
        test_std::start(state0);
        auto state1{test_std::connect(s1, receiver{})};
        test::use(state1);
        test_std::start(state1);
    }

    template <std::size_t Size>
    struct inline_resource
        : std::pmr::memory_resource
    {
        std::byte  buffer[Size];
        std::byte* next{+this->buffer};

        void* do_allocate(std::size_t size, std::size_t) override {
            if (size <= std::size_t(std::distance(next, std::end(buffer)))) {
                std::byte* rc{this->next};
                this->next += size;
                return rc;
            }
            return nullptr;
        }
        void do_deallocate(void*, std::size_t, std::size_t) override {
        }
        bool do_is_equal(std::pmr::memory_resource const& other) const noexcept override {
            return this == &other;
        }
    };

    namespace ex = test_std;

    struct fun
    {
        std::pmr::polymorphic_allocator<> allocator{};
        fun() {}
        fun(std::pmr::polymorphic_allocator<> allocator): allocator(allocator) {}
        auto operator()(std::span<int> s) noexcept
        {
            return ex::just(
                std::pmr::vector<int>(s.begin(), s.end(), this->allocator)
            );
        }
    };

    auto test_let_value_allocator() -> void
    {
        std::cout << "test_let_value_allocator()\n";
        std::vector<int> values{1, 2, 3};
        auto s{
            ex::just(std::span(values))
            | ex::let_value(fun())
            | ex::then([](auto&& v) noexcept {
                for (auto&& x: v) { std::cout << x << " "; }
                std::cout << "done\n";
            })
        };
        static_assert(test_std::sender<decltype(s)>);
        static_assert(test_std::sender_in<decltype(s)>);
        //static_assert(std::same_as<void, decltype(test_std::get_completion_signatures(s, test_std::empty_env{}))>);
        ex::sync_wait(std::move(s));
    }
}

// ----------------------------------------------------------------------------

auto main() -> int
{
    static_assert(std::same_as<test_std::let_error_t const, decltype(test_std::let_error)>);
    static_assert(std::same_as<test_std::let_stopped_t const, decltype(test_std::let_stopped)>);
    static_assert(std::same_as<test_std::let_value_t const, decltype(test_std::let_value)>);

    test_let_value();
    test_let_value_allocator();

    return EXIT_SUCCESS;
}