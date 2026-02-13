# BSP 原子操作模块（bsp_atomic.hpp）

## 原理
该模块基于 GCC 原子内建函数提供轻量级原子操作封装，用于在无 C++11 标准库原子支持或受限环境下实现线程安全的计数与状态修改。根据宏开关选择 __atomic 或 __sync 内建函数路径，在不引入标准库原子时仍可保证并发安全。

## 实现思想
- 通过 `memory_order` 枚举封装内存序，直接映射到编译器内建语义。
- `atomic<T>` 仅支持整型、指针、bool 等可原子操作类型；对非可原子类型使用自旋锁保护。
- 通过 `atomic_traits` 暴露 `is_always_lock_free` 信息，便于上层做性能判断。
- 使用模板特化区分指针、bool 与通用类型的实现路径。

## 典型场景
- 计数器（统计运行次数、包计数等）。
- 简单状态机的原子状态切换。
- 引用计数（见 `shared_ptr` 模块）。

## 如何使用（完整示例）
下面示例展示了原子计数与简单状态标志的用法：

```cpp
#include "bsp_atomic.hpp"

// 全局原子计数器
gdut::atomic<std::uint32_t> g_counter{0};

// 原子标志
gdut::atomic_bool g_ready{false};

void producer() {
  // 标记就绪
  g_ready.store(true, gdut::memory_order_release);
}

void consumer() {
  // 等待就绪（简单轮询示例）
  while (!g_ready.load(gdut::memory_order_acquire)) {
    osThreadYield();
  }

  // 计数递增
  g_counter.fetch_add(1, gdut::memory_order_relaxed);
}

uint32_t get_count() {
  return g_counter.load(gdut::memory_order_relaxed);
}
```

## 与代码规范的对应
- 遵循蛇形命名与类型名风格（如 `memory_order_seq_cst`）。
- 无 C 风格强转，使用显式类型系统与模板特化实现类型安全。
- 通过封装限制全局状态暴露，便于维护与复用。

## 注意事项/坑点
- 非整型/指针类型使用自旋锁实现，可能带来忙等开销。
- 忙等等待时建议主动让出（如 `osThreadYield()`），避免饿死低优先级线程。
- `memory_order` 选择错误可能导致可见性问题，需结合场景使用。
- 不要用原子替代互斥锁保护复杂对象。

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_atomic.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_atomic.hpp)
