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

struct shared_test_obj : gdut::enable_shared_from_this<shared_test_obj> {
  int value;
  shared_test_obj(int v) : value(v) {}
  void test() {
    auto sp = shared_from_this();
    // 这里可以安全地使用 sp 来访问对象
  }
};

void test_shared_ptr() {
  using allocator_type = gdut::pmr::polymorphic_allocator<>;
  auto sp1 = gdut::shared_ptr<int>(
      allocator_type{}.new_object<int>(42)); // 使用默认删除器
  auto sp2 = sp1;                            // 复制构造
  auto sp3 = std::move(sp1);                 // 移动构造
  auto sp4 = gdut::shared_ptr<int>(allocator_type{}.new_object<int>(100),
                                   [](int *p) { delete p; }); // 自定义删除器
  auto sp5 = gdut::shared_ptr<int>(sp4);                      // 复制构造
  auto sp6 = gdut::shared_ptr<int>(std::move(sp4));           // 移动构造
  gdut::weak_ptr<int> wp1 = sp5; // 从 shared_ptr 创建 weak_ptr
  auto sp7 = wp1.lock();         // 从 weak_ptr 锁定 shared_ptr
  gdut::shared_ptr<shared_test_obj> obj_sp = gdut::shared_ptr<shared_test_obj>(
      allocator_type{}.new_object<shared_test_obj>(123));
  obj_sp->test(); // 测试 enable_shared_from_this 支持
}

void test_thread() {
  gdut::thread<128> t([]() {
    // 线程函数
    for (int i = 0; i < 5; ++i) {
      // 模拟工作
      osDelay(100);
    }
  });
  if (t.joinable()) {
    t.join();
  }
}

int main() {
  test_shared_ptr();
  test_thread();
  return 0;
}
