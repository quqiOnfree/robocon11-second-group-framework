# BSP 内存资源模块（bsp_memory_resource.hpp）

该模块提供一组基于 `std::pmr::memory_resource` 的内存资源实现，面向 FreeRTOS 与 CMSIS-RTOS2 场景，统一通过标准库 PMR 接口进行分配与释放。所有资源类型均继承 `std::pmr::memory_resource`，可直接与 `std::pmr::polymorphic_allocator` 以及 PMR 容器协作。

## 资源类型

### 1. portable_resource
- 使用 FreeRTOS `pvPortMalloc/vPortFree` 作为上游分配器。
- 通过 `get_instance()` 获取单例。
- 当请求对齐大于 `portBYTE_ALIGNMENT` 时返回空指针（避免未对齐内存）。

### 2. unsynchronized_tlsf_resource
- 基于 TLSF 的非线程安全内存池资源。
- 通过上游资源申请池内存；超大分配自动回退到上游资源。
- 适用于单线程或外部已保证互斥的场景。

### 3. synchronized_tlsf_resource
- 线程安全版本的 TLSF 内存池资源。
- 内部使用互斥锁保护 `unsynchronized_tlsf_resource`。

### 4. os_memory_pool_resource
- 封装 CMSIS-RTOS2 的 `osMemoryPool`。
- 分配尺寸超过块大小时返回空指针。

## 使用示例

### 1. 将资源用于 PMR 容器

```cpp
#include <memory_resource>
#include <vector>
#include "bsp_memory_resource.hpp"

void example_pmr_vector() {
    gdut::pmr::synchronized_tlsf_resource pool;
    std::pmr::vector<int> v{&pool};
    v.push_back(1);
    v.push_back(2);
}
```

### 2. 使用 polymorphic_allocator

```cpp
#include <memory_resource>
#include "bsp_memory_resource.hpp"

struct node {
    int value;
};

void example_allocator() {
    gdut::pmr::portable_resource *upstream =
        static_cast<gdut::pmr::portable_resource *>(
            gdut::pmr::portable_resource::get_instance());

    std::pmr::polymorphic_allocator<node> alloc(upstream);
    node *p = alloc.allocate(1);
    p->value = 42;
    alloc.deallocate(p, 1);
}
```

## 注意事项

- `portable_resource` 仅保证 `portBYTE_ALIGNMENT` 对齐，超出对齐要求会返回空指针。
- `unsynchronized_tlsf_resource` / `synchronized_tlsf_resource` 对于超大请求会回退到上游资源。
- `os_memory_pool_resource` 仅支持不超过 `block_size` 的分配请求。

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_memory_resource.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_memory_resource.hpp)