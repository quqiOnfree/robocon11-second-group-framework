# BSP 内存池模块（bsp_memorypool.hpp）

## 原理
该模块封装 CMSIS-RTOS2 的内存池接口，提供固定块大小的分配器，适用于嵌入式环境的可预测内存管理。

## 实现思想
- `allocator<T, MaxSize>` 使用 OS 内存池，提供 `allocate/deallocate`。
- `mutexd_allocator` 在多线程场景下用互斥锁保护分配与释放。
- `pmr::polymorphic_allocator` 与 `memory_resource` 提供类 PMR 的扩展接口，支持不同内存资源。

## PMR 的处理方式
该模块实现了简化版 PMR（Polymorphic Memory Resource）机制，用于统一不同内存来源的分配方式：
- `pmr::memory_resource`：抽象基类，提供 `allocate/deallocate` 的虚函数接口，统一对齐与字节数处理。
- `pmr::default_memory_resource`：默认资源，内部使用 `pvPortMalloc/vPortFree`，适配 FreeRTOS 堆。
- `pmr::new_delete_resource`：使用 C++ `operator new/delete` 的资源实现，适合非 RTOS 或调试场景。
- `pmr::polymorphic_allocator`：面向类型的分配器封装，按 `Ty` 的对齐和大小向 `memory_resource` 申请内存，并提供 `new_object/delete_object` 便捷构造/销毁接口。

在本 BSP 中，`shared_ptr` 等组件通过 `pmr::polymorphic_allocator` 申请引用计数与删除器包装对象，从而实现与内存来源解耦。

## 如何使用
- 直接使用分配器（完整流程）：

```cpp
#include "bsp_memorypool.hpp"

struct node {
  int value;
  explicit node(int v) : value(v) {}
};

gdut::allocator<node, 32> pool;

void pool_demo() {
  node* p = pool.allocate();
  if (p == nullptr) {
    return;
  }

  gdut::allocator<node, 32>::construct(p, 7);
  // 使用对象
  int v = p->value;
  (void)v;

  gdut::allocator<node, 32>::destroy(p);
  pool.deallocate(p);
}
```

- 多线程安全分配：

```cpp
#include "bsp_memorypool.hpp"

gdut::mutexd_allocator<node, 16> safe_pool;

void safe_alloc_demo() {
  node* p = safe_pool.allocate();
  if (p) {
    gdut::mutexd_allocator<node, 16>::construct(p, 1);
    gdut::mutexd_allocator<node, 16>::destroy(p);
    safe_pool.deallocate(p);
  }
}
```

## 与代码规范的对应
- 资源管理清晰，避免裸 `new/delete`。
- 多线程访问遵循锁保护。
- 接口命名使用蛇形风格。

## 注意事项/坑点
- 内存池创建失败时 `allocate` 会返回 `nullptr`，上层必须检查。
- `allocate(timeout)` 的超时单位为毫秒，子毫秒会被截断。
- `construct/destroy` 需要显式调用，分配的裸内存不会自动调用构造与析构。
- `mutexd_allocator` 只保护分配与释放，不保护对象内部并发访问。
- `pmr::default_memory_resource` 使用 RTOS 堆，内存碎片与实时性需评估。

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_memorypool.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_memorypool.hpp)
