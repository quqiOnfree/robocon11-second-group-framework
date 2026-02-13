#include "bsp_clock.hpp"
#include "bsp_gpio_pin.hpp"
#include "bsp_memorypool.hpp"
#include "bsp_mutex.hpp"
#include "bsp_semaphore.hpp"
#include "bsp_shared_ptr.hpp"
#include "bsp_thread.hpp"
#include "bsp_timer.hpp"
#include "bsp_type_traits.hpp"
#include "bsp_uncopyable.hpp"
#include <memory>
#include <atomic>
#include <cassert>
#include <thread>
#include <vector>

namespace
{

struct test_object
{
    static std::atomic<int> instance_count;

    int value;

    explicit test_object(int v) : value(v)
    {
        instance_count.fetch_add(1, std::memory_order_relaxed);
    }

    test_object(const test_object &other) : value(other.value)
    {
        instance_count.fetch_add(1, std::memory_order_relaxed);
    }

    test_object(test_object &&other) noexcept : value(other.value)
    {
        instance_count.fetch_add(1, std::memory_order_relaxed);
    }

    ~test_object()
    {
        instance_count.fetch_sub(1, std::memory_order_relaxed);
    }
};

std::atomic<int> test_object::instance_count{0};

void test_shared_ptr_construction_and_destruction()
{
    test_object::instance_count.store(0, std::memory_order_relaxed);

    {
        auto ptr = std::make_shared<test_object>(42);
        assert(ptr);
        assert(ptr->value == 42);
        assert(test_object::instance_count.load(std::memory_order_relaxed) == 1);
        assert(ptr.use_count() == 1);
    }

    assert(test_object::instance_count.load(std::memory_order_relaxed) == 0);
}

void test_shared_ptr_copy_and_move()
{
    test_object::instance_count.store(0, std::memory_order_relaxed);

    auto ptr1 = std::make_shared<test_object>(7);
    assert(ptr1.use_count() == 1);

    std::shared_ptr<test_object> ptr2 = ptr1;
    assert(ptr1.use_count() == 2);
    assert(ptr2.use_count() == 2);

    std::shared_ptr<test_object> ptr3 = std::move(ptr1);
    assert(!ptr1);
    assert(ptr3.use_count() == 2);

    ptr2.reset();
    assert(ptr3.use_count() == 1);

    ptr3.reset();
    assert(test_object::instance_count.load(std::memory_order_relaxed) == 0);
}

void test_weak_ptr_behavior()
{
    test_object::instance_count.store(0, std::memory_order_relaxed);

    std::weak_ptr<test_object> weak;

    {
        auto ptr = std::make_shared<test_object>(100);
        weak        = ptr;

        assert(!weak.expired());
        auto locked = weak.lock();
        assert(locked);
        assert(locked->value == 100);
        assert(locked.use_count() == 2);
    }

    assert(weak.expired());
    auto locked_empty = weak.lock();
    assert(!locked_empty);
    assert(test_object::instance_count.load(std::memory_order_relaxed) == 0);
}

struct enable_shared_from_this_object : public std::enable_shared_from_this<enable_shared_from_this_object>
{
    int value;

    explicit enable_shared_from_this_object(int v) : value(v) {}

    std::shared_ptr<enable_shared_from_this_object> get_self()
    {
        return shared_from_this();
    }
};

void test_enable_shared_from_this()
{
    auto ptr = std::make_shared<enable_shared_from_this_object>(5);
    assert(ptr);
    assert(ptr.use_count() == 1);

    auto self_ptr = ptr->get_self();
    assert(self_ptr);
    assert(self_ptr.get() == ptr.get());
    assert(ptr.use_count() == 2);
    assert(self_ptr.use_count() == 2);
}

void test_shared_ptr_thread_safety()
{
    constexpr int thread_count      = 8;
    constexpr int increments_per_thread = 1000;

    auto counter_ptr = std::make_shared<std::atomic<int>>(0);

    assert(counter_ptr.use_count() == 1);

    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    for (int i = 0; i < thread_count; ++i)
    {
        auto local_ptr = counter_ptr;
        threads.emplace_back([local_ptr]() mutable {
            for (int j = 0; j < increments_per_thread; ++j)
            {
                local_ptr->fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto &t : threads)
    {
        t.join();
    }

    assert(counter_ptr->load(std::memory_order_relaxed) == thread_count * increments_per_thread);
}

} // namespace

int main()
{
    test_shared_ptr_construction_and_destruction();
    test_shared_ptr_copy_and_move();
    test_weak_ptr_behavior();
    test_enable_shared_from_this();
    test_shared_ptr_thread_safety();

    return 0;
}
