// tests/beman/execution26/bounded-queue.test.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/bounded_queue.hpp>
#include <test/execution.hpp>
#include <concepts>
#include <string>
#include <thread>
#include <iostream> //-dk:TODO remove

// ----------------------------------------------------------------------------

namespace {
template <typename T>
auto test_close(auto one, auto two) {
    test_std::bounded_queue<T> queue(1);
    ASSERT(noexcept(const_cast<const test_std::bounded_queue<T>&>(queue).is_closed()));
    ASSERT(not queue.is_closed());
    std::error_code ec{};
    ASSERT(queue.push(one, ec));
    auto val(queue.pop(ec));
    ASSERT(val.has_value());
    ASSERT(*val == one);

    ASSERT(noexcept(queue.close()));
    queue.close();
    ASSERT(queue.is_closed());

    ASSERT(&ec.category() != &test_std::conqueue_category());
    ASSERT(not queue.push(two, ec));
    ASSERT(ec.value() == static_cast<int>(test_std::conqueue_errc::closed));
    ASSERT(&ec.category() == &test_std::conqueue_category());

    ec = std::error_code();
    ASSERT(&ec.category() != &test_std::conqueue_category());
    auto noval(queue.pop(ec));
    ASSERT(not noval.has_value());
    ASSERT(ec.value() == static_cast<int>(test_std::conqueue_errc::closed));
    ASSERT(&ec.category() == &test_std::conqueue_category());
}

template <typename T>
auto test_push(const auto one, auto two, const auto three, auto four, auto five) -> void {
    test_std::bounded_queue<T> queue(4);
    queue.push(one);
    queue.push(std::move(two));
    std::error_code code{};
    bool            rc1{queue.push(three, code)};
    ASSERT(rc1 == true);
    bool rc2{queue.push(std::move(four), code)};
    ASSERT(rc2 == true);

    queue.close();
    try {
        queue.push(five);
        ASSERT(false);
    } catch (const test_std::conqueue_error& error) {
        ASSERT(error.code().value() == static_cast<int>(test_std::conqueue_errc::closed));
    }
    try {
        queue.push(std::move(five));
        ASSERT(false);
    } catch (const test_std::conqueue_error& error) {
        ASSERT(error.code().value() == static_cast<int>(test_std::conqueue_errc::closed));
    }
    std::error_code ec0{};
    ASSERT(queue.push(five, ec0) == false);
    ASSERT(ec0.value() == static_cast<int>(test_std::conqueue_errc::closed));

    std::error_code ec1{};
    ASSERT(queue.push(std::move(five), ec1) == false);
    ASSERT(ec1.value() == static_cast<int>(test_std::conqueue_errc::closed));
}

template <typename T>
auto test_pop(const auto one, auto two, const auto three, auto four, auto five) -> void {
    test_std::bounded_queue<T> queue(4);
    queue.push(one);
    queue.push(two);
    queue.push(three);
    queue.push(four);

    ASSERT(one == queue.pop());
    ASSERT(two == queue.pop());
    std::error_code ec;
    auto            val1(queue.pop(ec));
    ASSERT(val1.has_value());
    ASSERT(*val1 == three);
    auto val2(queue.try_pop(ec));
    ASSERT(val2.has_value());
    ASSERT(*val2 == four);

    auto val3(queue.try_pop(ec));
    ASSERT(not val3.has_value());
    ASSERT(&ec.category() == &test_std::conqueue_category());
    ASSERT(ec.value() == static_cast<int>(test_std::conqueue_errc::empty));

    {
        std::thread t([&] {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
            queue.push(five);
        });

        auto val4(queue.pop());
        ASSERT(val4 == five);
        t.join();
    }
    {
        std::thread t([&] {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
            queue.push(five);
        });

        std::error_code ec{};
        auto            val4(queue.pop(ec));
        ASSERT(val4.has_value());
        ASSERT(*val4 == five);
        t.join();
    }
    {
        std::thread t([&] {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
            queue.close();
        });

        try {
            queue.pop();
            ASSERT(false);
        } catch (const test_std::conqueue_error& ex) {
            ASSERT(&ex.code().category() == &test_std::conqueue_category());
            ASSERT(ex.code().value() == static_cast<int>(test_std::conqueue_errc::closed));
        }
        t.join();
    }
    {
        std::thread t([&] {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
            queue.close();
        });

        std::error_code ec{};
        auto            val4(queue.pop(ec));
        ASSERT(not val4.has_value());
        ASSERT(ec.value() == static_cast<int>(test_std::conqueue_errc::closed));
        t.join();
    }
}

template <typename T>
auto test_async_push(auto one, auto two, auto three, auto four, auto five) -> void {
    struct receiver {
        using receiver_concept = test_std::receiver_t;
        int& complete;
        auto set_value() && noexcept -> void { this->complete = 1; }
        auto set_error(test_std::conqueue_errc) && noexcept -> void { this->complete = 2; }
        auto set_stopped() && noexcept -> void { this->complete = 3; }
    };
    static_assert(test_std::receiver<receiver>);

    test_std::bounded_queue<T> queue(2);
    auto                       s4{queue.async_push(four)}; // verify that the order isn't call but start()
    auto                       s1{queue.async_push(one)};
    auto                       s2{queue.async_push(two)};
    auto                       s3{queue.async_push(three)};
    auto                       s5{queue.async_push(five)};

    int c1{}, c2{}, c3{}, c4{}, c5{};

    auto op2{test_std::connect(std::move(s2), receiver{c2})}; // connect order also doesn't matter
    ASSERT(c2 == 0);
    auto op1{test_std::connect(std::move(s1), receiver{c1})};
    ASSERT(c1 == 0);
    auto op3{test_std::connect(std::move(s3), receiver{c3})};
    ASSERT(c3 == 0);
    auto op4{test_std::connect(std::move(s4), receiver{c4})};
    ASSERT(c4 == 0);
    auto op5{test_std::connect(std::move(s5), receiver{c5})};
    ASSERT(c5 == 0);

    test_std::start(op1);
    ASSERT(c1 == 1);
    test_std::start(op2);
    ASSERT(c2 == 1);
    test_std::start(op3);
    ASSERT(c3 == 0);
    test_std::start(op4);
    ASSERT(c4 == 0);
    test_std::start(op5);
    ASSERT(c5 == 0);

    ASSERT(queue.pop() == one);
    ASSERT(c3 == 1);
    ASSERT(queue.pop() == two);
    ASSERT(c4 == 1);
    queue.close();
    ASSERT(c5 == 2);
}

template <typename T>
auto test_async_pop(auto one, auto two, auto three, auto four, auto five) -> void {
    struct receiver {
        using receiver_concept = test_std::receiver_t;
        int&            complete;
        std::vector<T>& vals;
        auto set_value(T val) && noexcept -> void { this->complete = 1; vals.push_back(val); }
        auto set_error(test_std::conqueue_errc) && noexcept -> void { this->complete = 2; }
        auto set_stopped() && noexcept -> void { this->complete = 3; }
    };
    static_assert(test_std::receiver<receiver>);

    test_std::bounded_queue<T> queue(2);
    std::vector<T>             vals;
    queue.push(one);
    queue.push(two);

    auto s4{queue.async_pop()};
    auto s2{queue.async_pop()};
    auto s1{queue.async_pop()};
    auto s3{queue.async_pop()};
    auto s5{queue.async_pop()};

    int c1{}, c2{}, c3{}, c4{}, c5{};

    auto op4{test_std::connect(std::move(s4), receiver{c4, vals})};
    ASSERT(c4 == 0);
    auto op2{test_std::connect(std::move(s2), receiver{c2, vals})};
    ASSERT(c2 == 0);
    auto op1{test_std::connect(std::move(s1), receiver{c1, vals})};
    ASSERT(c1 == 0);
    auto op3{test_std::connect(std::move(s3), receiver{c3, vals})};
    ASSERT(c3 == 0);
    auto op5{test_std::connect(std::move(s5), receiver{c5, vals})};
    ASSERT(c5 == 0);

    test_std::start(op1);
    ASSERT(c1 == 1);
    ASSERT(vals.size() == 1u);
    ASSERT(vals.back() == one);

    test_std::start(op2);
    ASSERT(c2 == 1);
    ASSERT(vals.size() == 2u);
    ASSERT(vals.back() == two);

    test_std::start(op3);
    ASSERT(c3 == 0);
    ASSERT(vals.size() == 2u);

    test_std::start(op4);
    ASSERT(c4 == 0);
    ASSERT(vals.size() == 2u);

    test_std::start(op5);
    ASSERT(c5 == 0);
    ASSERT(vals.size() == 2u);

    queue.push(three);
    ASSERT(c3 == 1);
    ASSERT(vals.size() == 3u);
    ASSERT(vals.back() == three);

    queue.push(four);
    ASSERT(c4 == 1);
    ASSERT(vals.size() == 4u);
    ASSERT(vals.back() == four);

    queue.close();
    ASSERT(c5 == 2);
    ASSERT(vals.size() == 4u);
}

} // namespace

TEST(bounded_queue) {
    using namespace std::string_literals;

    static_assert(not std::movable<test_std::bounded_queue<int>>);
    static_assert(not std::copyable<test_std::bounded_queue<int>>);
    static_assert(std::same_as<int, test_std::bounded_queue<int>::value_type>);
    static_assert(std::same_as<std::allocator<int>, test_std::bounded_queue<int>::allocator_type>);

    test_close<int>(1, 2);
    test_close<std::string>("one"s, "two"s);

    test_push<int>(1, 2, 3, 4, 5);
    test_push<std::string>("one"s, "two"s, "three"s, "four"s, "five"s);

    test_pop<int>(1, 2, 3, 4, 5);
    test_pop<std::string>("one"s, "two"s, "three"s, "four"s, "five"s);

    test_async_push<int>(1, 2, 3, 4, 5);
    test_async_push<std::string>("one"s, "two"s, "three"s, "four"s, "five"s);
    test_async_pop<int>(1, 2, 3, 4, 5);
    test_async_pop<std::string>("one"s, "two"s, "three"s, "four"s, "five"s);
}